/*
 * Parse/ParseExpr.cpp
 * © suhas pai
 */

#include <cassert>
#include <format>

#include "AST/Decls/ArrayDecl.h"

#include "AST/Types/ArrayType.h"
#include "AST/Types/OptionalType.h"
#include "AST/Types/PointerType.h"

#include "AST/ArraySubscriptExpr.h"
#include "AST/BinaryOperation.h"
#include "AST/CallExpr.h"
#include "AST/CastExpr.h"
#include "AST/DeclRefExpr.h"
#include "AST/DerefExpr.h"
#include "AST/DotIdentifierExpr.h"
#include "AST/FieldExpr.h"
#include "AST/NumberLiteral.h"
#include "AST/OptionalUnwrapExpr.h"
#include "AST/ParenExpr.h"
#include "AST/UnaryOperation.h"

#include "Parse/OperatorPrecedence.h"

#include "Parse/ParseDecl.h"
#include "Parse/ParseExpr.h"
#include "Parse/ParseIfExpr.h"
#include "Parse/ParseMisc.h"
#include "Parse/ParseString.h"

#include "llvm/Support/Casting.h"

namespace Parse {
    [[nodiscard]] static auto
    ParseArrayDetailList(ParseContext &Context,
                         const Lex::Token BracketToken) noexcept
        -> std::expected<std::vector<AST::Stmt *>, ParseError>
    {
        auto &TokenStream = Context.TokenStream;
        auto DetailList = std::vector<AST::Stmt *>();

        if (TokenStream.consumeIfIs(Lex::TokenKind::RightSquareBracket)) {
            return DetailList;
        }

        auto &Diag = Context.Diag;
        do {
            if (const auto ExprOpt = ParseExpression(Context)) {
                DetailList.emplace_back(ExprOpt.value());
                if (TokenStream.consumeIfIs(Lex::TokenKind::Comma)) {
                    continue;
                }

                if (TokenStream.consumeIfIs(Lex::TokenKind::RightSquareBracket))
                {
                    break;
                }

                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getEofLocation(),
                    .Message = "Expected ']'",
                });
            } else {
                return std::unexpected(ExprOpt.error());
            }

            const auto ProceedResult =
                ProceedToAndConsumeCommaOrEnd(
                    Context, Lex::TokenKind::RightSquareBracket);

            switch (ProceedResult) {
                case ProceedResult::Failed:
                    Diag.consume({
                        .Level = DiagnosticLevel::Error,
                        .Location = BracketToken.Loc,
                        .Message = "Expected ']'",
                    });

                    return std::unexpected(ParseError::FailedCouldNotProceed);
                case ProceedResult::Comma:
                    continue;
                case ProceedResult::EndToken:
                    goto done;
            }
        } while (true);

    done:
        return DetailList;
    }

    [[nodiscard]]
    static auto IsLikelyParamList(Lex::TokenStream &TokenStream) noexcept
        -> std::optional<bool>
    {
        if (TokenStream.consumeIfIs(Lex::TokenKind::CloseParen)) {
            if (TokenStream.peekIs(Lex::TokenKind::ThinArrow) ||
                TokenStream.peekIs(Lex::TokenKind::FatArrow) ||
                TokenStream.peekIs(Lex::TokenKind::Colon))
            {
                TokenStream.goBack();
                return true;
            }

            TokenStream.goBack();
            return std::nullopt;
        }

        if (TokenStream.consumeIfIs(Lex::TokenKind::Identifier)) {
            if (TokenStream.peekIs(Lex::TokenKind::Colon) ||
                TokenStream.peekIs(Lex::TokenKind::Equal))
            {
                TokenStream.goBack();
                return true;
            }

            if (TokenStream.consumeIfIs(Lex::TokenKind::CloseParen)) {
                if (TokenStream.peekIs(Lex::TokenKind::ThinArrow) ||
                    TokenStream.peekIs(Lex::TokenKind::FatArrow) ||
                    TokenStream.peekIs(Lex::TokenKind::Colon) ||
                    TokenStream.peekIs(Lex::TokenKind::LeftSquareBracket))
                {
                    TokenStream.goBack(2);
                    return true;
                }

                TokenStream.goBack();
            }

            TokenStream.goBack();
        }

        const auto Result =
            TokenStream.inWindow([](Lex::TokenStream &TokenStream) noexcept {
                const auto TokenOpt =
                    TokenStream.findNextAndConsume(Lex::TokenKind::CloseParen);

                if (!TokenOpt.has_value()) {
                    return false;
                }

                const auto NextTokenOpt = TokenStream.peek();
                if (!NextTokenOpt.has_value()) {
                    return false;
                }

                const auto NextToken = NextTokenOpt.value();
                if (NextToken.Kind == Lex::TokenKind::ThinArrow ||
                    NextToken.Kind == Lex::TokenKind::FatArrow ||
                    NextToken.Kind == Lex::TokenKind::Colon)
                {
                    return true;
                }

                return false;
            });

        return Result;
    }

    [[nodiscard]] static auto
    ParseExprWithSquareBracket(ParseContext &Context,
                               const Lex::Token BracketToken) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        auto &TokenStream = Context.TokenStream;
        const auto IsClosureCaptureList =
            TokenStream.inWindow([](Lex::TokenStream &TokenStream) noexcept {
                const auto TokenOpt =
                    TokenStream.findNextAndConsume(
                        Lex::TokenKind::RightSquareBracket);

                if (!TokenOpt.has_value()) {
                    return false;
                }

                const auto NextTokenOpt = TokenStream.peek();
                if (!NextTokenOpt.has_value()) {
                    return false;
                }

                const auto NextToken = NextTokenOpt.value();
                if (NextToken.Kind != Lex::TokenKind::OpenParen) {
                    return false;
                }

                TokenStream.consume();
                if (const auto IsParamListOpt = IsLikelyParamList(TokenStream))
                {
                    return IsParamListOpt.value();
                }

                return false;
            });

        if (IsClosureCaptureList) {
            return ParseClosureDecl(Context, BracketToken);
        }

        auto DetailListOpt = ParseArrayDetailList(Context, BracketToken);
        if (!DetailListOpt.has_value()) {
            return std::unexpected(DetailListOpt.error());
        }

        // We have two possibilities for the kinds of expressions we have:
        //  (a) We have an array
        //  (b) We have an array-type

        // (a) is the case where we have an array-type.
        //
        // Here, we expect an expression that is not a binary operation or
        // expression or a member-access expression (a dot-identifier token).

        if (const auto PeekTokenOpt = TokenStream.peek()) {
            const auto PeekToken = PeekTokenOpt.value();
            if (!TokenStream.tokenIsBinOp(PeekToken) &&
                PeekToken.Kind != Lex::TokenKind::DotIdentifier)
            {
                // If we have a question-mark, we need to check if the
                // programmer intended to use an optional-type or if they
                // (mistakenly) used a question-mark to unwrap a non-optional
                // array.
                //
                // If they intended to use an optional-type, we need to parse
                // the expression after the question-mark as the operand of
                // the optional-type.
                //
                // If they (mistakenly) used a question-mark to unwrap a
                // non-optional array, we need to parse the expression after
                // the question-mark as a simple array.

                if (PeekToken.Kind == Lex::TokenKind::QuestionMark) {
                    TokenStream.consume();

                    const auto PeekToken2Opt = TokenStream.peek();
                    if (!PeekToken2Opt.has_value()) {
                        TokenStream.goBack();

                        auto &DetailList = DetailListOpt.value();
                        return new AST::ArrayDecl(BracketToken.Loc,
                                                  std::move(DetailList));
                    }

                    const auto PeekToken2 = PeekToken2Opt.value();
                    if (TokenStream.tokenIsBinOp(PeekToken2) ||
                        PeekToken2.Kind == Lex::TokenKind::DotIdentifier)
                    {
                        TokenStream.goBack();

                        auto &DetailList = DetailListOpt.value();
                        return new AST::ArrayDecl(BracketToken.Loc,
                                                  std::move(DetailList));
                    }
                }

                // We have an array-type
                const auto BaseOpt = ParseLhs(Context, /*InPlaceOfStmt=*/false);
                if (!BaseOpt.has_value()) {
                    return std::unexpected(BaseOpt.error());
                }

                auto Base = BaseOpt.value();
                auto &DetailList = DetailListOpt.value();

                return new AST::ArrayTypeExpr(BracketToken.Loc,
                                              std::move(DetailList), Base,
                                              AST::Qualifiers());
            }
        }

        auto &DetailList = DetailListOpt.value();
        return new AST::ArrayDecl(BracketToken.Loc, std::move(DetailList));
    }

    [[nodiscard]] static auto
    ParseArraySubscriptExpr(ParseContext &Context,
                            AST::Expr *const Lhs,
                            const Lex::Token BracketToken) noexcept
        -> std::expected<AST::ArraySubscriptExpr *, ParseError>
    {
        auto DetailListOpt = ParseArrayDetailList(Context, BracketToken);
        if (!DetailListOpt.has_value()) {
            return std::unexpected(DetailListOpt.error());
        }

        auto &DetailList = DetailListOpt.value();
        return new AST::ArraySubscriptExpr(BracketToken.Loc, Lhs,
                                           std::move(DetailList));
    }

    [[nodiscard]] static auto
    ParseFieldExpr(ParseContext &Context,
                   AST::Expr *const Lhs,
                   const Lex::Token DotToken)
        -> std::expected<AST::Expr *, ParseError>
    {
        if (DotToken.Kind == Lex::TokenKind::DotStar) {
            return new AST::DerefExpr(DotToken.Loc, Lhs);
        }

        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto MemberToken = Lex::Token();
        if (DotToken.Kind != Lex::TokenKind::DotIdentifier) {
            const auto MemberTokenOpt =
                TokenStream.consumeIfIs(Lex::TokenKind::Identifier);

            if (!MemberTokenOpt.has_value()) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = DotToken.Loc,
                    .Message = "Expected member name"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }

            MemberToken = MemberTokenOpt.value();
        } else {
            MemberToken = DotToken;
        }

        // Accept certain tokens as member names, error out later
        if (MemberToken.Kind != Lex::TokenKind::Identifier &&
            MemberToken.Kind != Lex::TokenKind::IntegerLiteral &&
            MemberToken.Kind != Lex::TokenKind::DotIdentifier)
        {
            const auto MemberString = TokenStream.tokenContent(MemberToken);
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = MemberToken.Loc,
                .Message =
                    std::format("Expected member name, got '{}' instead",
                                MemberString)
            });

            return std::unexpected(ParseError::FailedAndProceeded);
        }

        auto MemberName = TokenStream.tokenContent(MemberToken);
        if (MemberToken.Kind == Lex::TokenKind::DotIdentifier) {
            MemberName = MemberName.substr(1);
        }

        const auto IsArrow = DotToken.Kind == Lex::TokenKind::ThinArrow;
        return new AST::FieldExpr(MemberToken.Loc, Lhs, IsArrow,
                                  std::move(MemberName));
    }

    [[nodiscard]] static auto
    ParseCallExpr(ParseContext &Context,
                  AST::Expr *const CalleeExpr,
                  const AST::Qualifiers &Qualifiers,
                  const Lex::Token ParenToken) noexcept
        -> std::expected<AST::CallExpr *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto ArgList = std::vector<AST::CallExpr::Argument>();
        if (TokenStream.consumeIfIs(Lex::TokenKind::CloseParen)) {
            goto done;
        }

        do {
            auto ArgNameOpt = std::optional<std::string_view>(std::nullopt);
            if (const auto IdentTokenOpt =
                    TokenStream.consumeIfIs(Lex::TokenKind::Identifier))
            {
                if (TokenStream.consumeIfIs(Lex::TokenKind::Colon)) {
                    ArgNameOpt =
                        TokenStream.tokenContent(IdentTokenOpt.value());
                } else {
                    TokenStream.goBack();
                }
            }

            if (const auto ExprOpt = ParseExpression(Context)) {
                ArgList.emplace_back(ArgNameOpt, ExprOpt.value());
                if (TokenStream.consumeIfIs(Lex::TokenKind::Comma)) {
                    continue;
                }

                if (TokenStream.consumeIfIs(Lex::TokenKind::CloseParen)) {
                    break;
                }

                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = ParenToken.Loc,
                    .Message = "Expected ',' or ')'",
                });
            }

            const auto ProceedResult =
                ProceedToAndConsumeCommaOrEnd(Context,
                                              Lex::TokenKind::CloseParen);

            switch (ProceedResult) {
                case ProceedResult::Failed:
                    Diag.consume({
                        .Level = DiagnosticLevel::Error,
                        .Location = ParenToken.Loc,
                        .Message = "Expected ',' or ')'",
                    });

                    return std::unexpected(ParseError::FailedCouldNotProceed);
                case ProceedResult::Comma:
                    continue;
                case ProceedResult::EndToken:
                    goto done;
            }
        } while (true);

done:
        return new AST::CallExpr(CalleeExpr, ParenToken.Loc, Qualifiers,
                                 std::move(ArgList));
    }

    [[nodiscard]] static auto
    ParseCallAndFieldExprs(ParseContext &Context,
                           AST::Expr *Root,
                           const AST::Qualifiers &Quals) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        const auto FieldTokenList = {
            Lex::TokenKind::Dot,
            Lex::TokenKind::DotIdentifier,
            Lex::TokenKind::DotStar,
            Lex::TokenKind::ThinArrow
        };

        auto &TokenStream = Context.TokenStream;
        while (true) {
            if (TokenStream.peekIsOneOf(FieldTokenList)) {
                const auto Token = TokenStream.consume().value();
                if (const auto Result = ParseFieldExpr(Context, Root, Token)) {
                    Root = Result.value();
                } else {
                    return std::unexpected(Result.error());
                }

                continue;
            }

            if (!llvm::isa<AST::OptionalTypeExpr>(Root) &&
                !llvm::isa<AST::PointerTypeExpr>(Root) &&
                !llvm::isa<AST::ArrayTypeExpr>(Root))
            {
                if (const auto TokenOpt =
                        TokenStream.consumeIfIs(Lex::TokenKind::QuestionMark))
                {
                    const auto Token = TokenOpt.value();
                    Root = new AST::OptionalUnwrapExpr(Token.Loc, Root);

                    continue;
                }
            }

            if (TokenStream.peekIs(Lex::TokenKind::LeftSquareBracket)) {
                const auto Token = TokenStream.consume().value();
                const auto Result =
                    ParseArraySubscriptExpr(Context, Root, Token);

                if (!Result.has_value()) {
                    return std::unexpected(Result.error());
                }

                Root = Result.value();
                continue;
            }

            if (TokenStream.peekIs(Lex::TokenKind::OpenParen)) {
                const auto Token = TokenStream.consume().value();
                const auto Result = ParseCallExpr(Context, Root, Quals, Token);

                if (!Result.has_value()) {
                    return std::unexpected(Result.error());
                }

                Root = Result.value();
                continue;
            }

            break;
        }

        return Root;
    }

    [[nodiscard]] static auto
    ParseIdentifierForLhs(ParseContext &Context,
                          AST::Qualifiers &Qualifiers,
                          const Lex::Token IdentToken) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        auto &TokenStream = Context.TokenStream;
        if (IdentToken.Kind == Lex::TokenKind::Identifier) {
            const auto IdentName = TokenStream.tokenContent(IdentToken);
            return new AST::DeclRefExpr(IdentName, IdentToken.Loc);
        }

        if (IdentToken.Kind == Lex::TokenKind::DotIdentifier) {
            const auto IdentName =
                TokenStream.tokenContent(IdentToken).substr(1);

            return new AST::DotIdentifierExpr(IdentToken.Loc,
                                              std::move(Qualifiers), IdentName);
        }

        __builtin_unreachable();
    }

    [[nodiscard]] static auto
    ParseKeywordForLhs(ParseContext &Context,
                       AST::Qualifiers &Qualifiers,
                       const Lex::Token KeywordTok,
                       const bool InPlaceOfStmt) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto Root = static_cast<AST::Expr *>(nullptr);
        auto NameOpt = std::optional<Lex::Token>(std::nullopt);

        const auto Keyword = TokenStream.tokenKeyword(KeywordTok);
        switch (Keyword) {
            case Lex::Keyword::Struct: {
                const auto Result =
                    ParseStructDecl(Context, KeywordTok,  NameOpt);

                if (!Result.has_value()) {
                    return std::unexpected(Result.error());
                }

                Root = Result.value();
                break;
            }
            case Lex::Keyword::Class:
                break;
            case Lex::Keyword::Shape: {
                const auto Result =
                    ParseShapeDecl(Context, KeywordTok, NameOpt);

                if (!Result.has_value()) {
                    return std::unexpected(Result.error());
                }

                Root = Result.value();
                break;
            }
            case Lex::Keyword::Union: {
                const auto Result =
                    ParseUnionDecl(Context, KeywordTok, NameOpt);

                if (!Result.has_value()) {
                    return std::unexpected(Result.error());
                }

                Root = Result.value();
                break;
            }
            case Lex::Keyword::Interface: {
                const auto Result =
                    ParseInterfaceDecl(Context, KeywordTok, NameOpt);

                if (!Result.has_value()) {
                    return std::unexpected(Result.error());
                }

                Root = Result.value();
                break;
            }
            case Lex::Keyword::Impl:
            case Lex::Keyword::Enum:
                //Root = ParseEnumDecl(Context, UnusedNameTokenOpt);
                if (Root == nullptr) {
                    return std::unexpected(ParseError::FailedCouldNotProceed);
                }

                break;
            case Lex::Keyword::If: {
                const auto Result =
                    ParseIfExpr(Context, KeywordTok, ParseExpression,
                                /*SeparatorOpt=*/std::nullopt);

                if (!Result.has_value()) {
                    return std::unexpected(Result.error());
                }

                Root = Result.value();
                break;
            }
            case Lex::Keyword::Function: {
                const auto Result =
                    ParseFunctionDecl(Context, KeywordTok, NameOpt);

                if (!Result.has_value()) {
                    return std::unexpected(Result.error());
                }

                Root = Result.value();
                break;
            }
            case Lex::Keyword::Let:
            case Lex::Keyword::Mut:
            case Lex::Keyword::Else:
            case Lex::Keyword::Return:
            case Lex::Keyword::Volatile:
            case Lex::Keyword::And:
            case Lex::Keyword::Or:
            case Lex::Keyword::For:
            case Lex::Keyword::While:
            case Lex::Keyword::Inline:
            case Lex::Keyword::Comptime:
            case Lex::Keyword::Default:
            case Lex::Keyword::In:
            case Lex::Keyword::Discardable:
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = KeywordTok.Loc,
                    .Message =
                        std::format("Keyword \"{}\" cannot be used in an "
                                    "expression",
                                    TokenStream.tokenContent(KeywordTok)),
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            case Lex::Keyword::As:
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = KeywordTok.Loc,
                    .Message = "Unexpected 'as'"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        if (!InPlaceOfStmt && NameOpt.has_value()) {
            const auto Name = NameOpt.value();
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = Name.Loc,
                .Message =
                    std::format("Name \"{}\" will be unused",
                                TokenStream.tokenContent(Name))
            });
        }

        return Root;
    }

    [[nodiscard]] static auto
    ParseExprWithOpenParen(ParseContext &Context,
                           const Lex::Token ParenToken) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        // We assume this could simply be a paren-expr, but we could instead
        // have an arrow-function's parameter-list, which we need to check.

        // The two possibilities that indicate a non-paren expr are an empty
        // parenthesis, or a parenthesis with an identifier.

        if (const auto IsParamListOpt = IsLikelyParamList(TokenStream)) {
            if (IsParamListOpt.value()) {
                const auto FuncOpt =
                    ParseArrowFunctionDeclOrFunctionType(Context, ParenToken);

                if (!FuncOpt.has_value()) {
                    return std::unexpected(FuncOpt.error());
                }

                return FuncOpt.value();
            }
        } else {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getEofLocation(),
                .Message = "Expected an expression inside parenthesis, or an "
                           "'->' for an function type or '=>' for an arrow "
                           "function"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto ChildExprOpt = ParseExpression(Context);
        if (!ChildExprOpt.has_value()) {
            if (!TokenStream.proceedToAndConsume(Lex::TokenKind::CloseParen)) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = ParenToken.Loc,
                    .Message = "Expected closing parenthesis"
                });
            }

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        if (!TokenStream.consumeIfIs(Lex::TokenKind::CloseParen)) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = ParenToken.Loc,
                .Message = "Expected closing parenthesis"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        return new AST::ParenExpr(ParenToken, ChildExprOpt.value());
    }

    [[nodiscard]] static auto
    ParseNumberLiteral(ParseContext &Context, const Lex::Token Token) noexcept
        -> AST::NumberLiteral *
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        const auto ParseResult = ParseNumber(TokenStream.tokenContent(Token));
        switch (ParseResult.Error.Kind) {
            case ParseNumberErrorKind::None:
                break;
            case ParseNumberErrorKind::EmptyString:
                assert(false && "NumberLiteral token-string is empty");
            case ParseNumberErrorKind::NegativeNumber:
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = Token.Loc,
                    .Message = "Signed integers can't be negative"
                });

                break;
            case ParseNumberErrorKind::PositiveSign:
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = Token.Loc,
                    .Message = "Integers can't have a +sign"
                });

                break;
            case ParseNumberErrorKind::InvalidDigit:
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location =
                        Token.Loc.adding(ParseResult.Error.IndexOfInvalid),
                    .Message = "Invalid digit in number"
                });

                break;
            case ParseNumberErrorKind::UnrecognizedChar:
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location =
                        Token.Loc.adding(ParseResult.Error.IndexOfInvalid),
                    .Message = "Unrecognized character in number"
                });

                break;
            case ParseNumberErrorKind::UnrecognizedBase:
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location =
                        Token.Loc.adding(ParseResult.Error.IndexOfInvalid),
                    .Message = "Unrecognized base in number"
                });

                break;
            case ParseNumberErrorKind::TooLarge:
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = Token.Loc,
                    .Message = "Number is too large"
                });

                break;
            case ParseNumberErrorKind::LeadingZero:
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = Token.Loc,
                    .Message = "Number cannot have a leading zero"
                });

                break;
            case ParseNumberErrorKind::NegativeZero:
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = Token.Loc,
                    .Message = "Negative zero cannot be valid"
                });

                break;
            case ParseNumberErrorKind::FloatingPoint:
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = Token.Loc,
                    .Message = "Floating-point number not supported"
                });

                break;
            case ParseNumberErrorKind::Overflow:
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = Token.Loc,
                    .Message = "Number is too large"
                });

                break;
        }

        return new AST::NumberLiteral(Token.Loc, ParseResult);
    }

    auto
    ParseUnaryOperation(ParseContext &Context, const Lex::Token Token) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        auto &TokenStream = Context.TokenStream;

        const auto TokenString = TokenStream.tokenContent(Token);
        const auto UnaryOpOpt = UnaryOperatorToLexemeMap.keyFor(TokenString);

        assert(UnaryOpOpt.has_value() &&
               "Unary operator not found in lexeme map");

        const auto UnaryOp = UnaryOpOpt.value();
        if (UnaryOp == UnaryOperator::Optional) {
            return new AST::OptionalTypeExpr(Token.Loc, /*Operand=*/nullptr);
        }

        if (UnaryOp == UnaryOperator::Pointer) {
            return new AST::PointerTypeExpr(Token.Loc, /*Operand=*/nullptr);
        }

        return new AST::UnaryOperation(Token.Loc, UnaryOp, /*Operand=*/nullptr);
    }

    auto ParseLhs(ParseContext &Context, const bool InPlaceOfStmt) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        // Parse Prefixes and then Lhs
        auto Root = static_cast<AST::Expr *>(nullptr);
        auto CurrentOper = static_cast<AST::Expr *>(nullptr);

        while (true) {
            auto Qualifiers = AST::Qualifiers();
            ParseQualifiers(Context, Qualifiers);

            const auto TokenOpt = TokenStream.consume();
            if (!TokenOpt.has_value()) {
                if (Root != nullptr) {
                    return Root;
                }

                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getEofLocation(),
                    .Message = "Expected an expression, but reached end of file"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }

            const auto Token = TokenOpt.value();
            const auto IsUnaryOp =  Lex::TokenKindIsUnaryOp(Token.Kind);

            auto Expr = static_cast<AST::Expr *>(nullptr);
            if (!IsUnaryOp && Token.Kind != Lex::TokenKind::Minus) {
                switch (Token.Kind) {
                    case Lex::TokenKind::Identifier:
                    case Lex::TokenKind::DotIdentifier: {
                        const auto Result =
                            ParseIdentifierForLhs(Context, Qualifiers, Token);

                        if (!Result.has_value()) {
                            return std::unexpected(Result.error());
                        }

                        Expr = Result.value();
                        break;
                    }
                    case Lex::TokenKind::Keyword: {
                        const auto Result =
                            ParseKeywordForLhs(Context, Qualifiers, Token,
                                               InPlaceOfStmt);

                        if (!Result.has_value()) {
                            return std::unexpected(Result.error());
                        }

                        Expr = Result.value();
                        break;
                    }
                    case Lex::TokenKind::IntegerLiteral:
                    case Lex::TokenKind::FloatLiteral:
                        Expr = ParseNumberLiteral(Context, Token);
                        break;
                    case Lex::TokenKind::CharLiteral:
                        Expr = ParseCharLiteral(Context, Token);
                        break;
                    case Lex::TokenKind::StringLiteral:
                        Expr = ParseStringLiteral(Context, Token);
                        break;
                    case Lex::TokenKind::OpenParen: {
                        const auto Result =
                            ParseExprWithOpenParen(Context, Token);

                        if (!Result.has_value()) {
                            return std::unexpected(Result.error());
                        }

                        Expr = Result.value();
                        break;
                    }
                    case Lex::TokenKind::LeftSquareBracket: {
                        const auto Result =
                            ParseExprWithSquareBracket(Context, Token);

                        if (!Result.has_value()) {
                            return std::unexpected(Result.error());
                        }

                        Expr = Result.value();
                        break;
                    }
                    case Lex::TokenKind::QuestionMark:
                    case Lex::TokenKind::Exclamation:
                    case Lex::TokenKind::Tilde:
                    case Lex::TokenKind::Ampersand:
                    case Lex::TokenKind::Star:
                    case Lex::TokenKind::DotDotDot:
                        // Unary-Operation tokens that should've never been
                        // reached

                        __builtin_unreachable();
                    case Lex::TokenKind::IntegerLiteralWithSuffix:
                    case Lex::TokenKind::FloatLiteralWithSuffix:
                    case Lex::TokenKind::Plus:
                    case Lex::TokenKind::Minus:
                    case Lex::TokenKind::PlusEqual:
                    case Lex::TokenKind::MinusEqual:
                    case Lex::TokenKind::DoubleStar:
                    case Lex::TokenKind::StarEqual:
                    case Lex::TokenKind::Slash:
                    case Lex::TokenKind::SlashEqual:
                    case Lex::TokenKind::Percent:
                    case Lex::TokenKind::PercentEqual:
                    case Lex::TokenKind::ShiftLeft:
                    case Lex::TokenKind::ShiftLeftEqual:
                    case Lex::TokenKind::ShiftRight:
                    case Lex::TokenKind::ShiftRightEqual:
                    case Lex::TokenKind::Caret:
                    case Lex::TokenKind::CaretEqual:
                    case Lex::TokenKind::AmpersandEqual:
                    case Lex::TokenKind::DoubleAmpersand:
                    case Lex::TokenKind::Pipe:
                    case Lex::TokenKind::PipeEqual:
                    case Lex::TokenKind::DoublePipe:
                    case Lex::TokenKind::TildeEqual:
                    case Lex::TokenKind::LessThan:
                    case Lex::TokenKind::GreaterThan:
                    case Lex::TokenKind::LessThanOrEqual:
                    case Lex::TokenKind::GreaterThanOrEqual:
                    case Lex::TokenKind::Equal:
                    case Lex::TokenKind::NotEqual:
                    case Lex::TokenKind::DoubleEqual:
                    case Lex::TokenKind::CloseParen:
                    case Lex::TokenKind::OpenCurlyBrace:
                    case Lex::TokenKind::CloseCurlyBrace:
                    case Lex::TokenKind::RightSquareBracket:
                    case Lex::TokenKind::Comma:
                    case Lex::TokenKind::Colon:
                    case Lex::TokenKind::Semicolon:
                    case Lex::TokenKind::Dot:
                    case Lex::TokenKind::DotStar:
                    case Lex::TokenKind::DotDot:
                    case Lex::TokenKind::DotDotLessThan:
                    case Lex::TokenKind::DotDotGreaterThan:
                    case Lex::TokenKind::DotDotEqual:
                    case Lex::TokenKind::ThinArrow:
                    case Lex::TokenKind::FatArrow:
                    case Lex::TokenKind::EOFToken:
                    case Lex::TokenKind::Invalid:
                        Diag.consume({
                            .Level = DiagnosticLevel::Error,
                            .Location = Token.Loc,
                            .Message =
                                std::format("Unexpected token \"{}\"",
                                            TokenStream.tokenContent(Token))
                        });

                        return std::unexpected(
                            ParseError::FailedCouldNotProceed);
                }
            } else {
                const auto Result = ParseUnaryOperation(Context, Token);
                if (!Result.has_value()) {
                    return std::unexpected(Result.error());
                }

                Expr = Result.value();
            }

            if (!llvm::isa<AST::OptionalTypeExpr>(Expr) &&
                !llvm::isa<AST::PointerTypeExpr>(Expr))
            {
                const auto ParseResult =
                    ParseCallAndFieldExprs(Context, Expr, Qualifiers);

                if (!ParseResult.has_value()) {
                    return std::unexpected(ParseResult.error());
                }

                Expr = ParseResult.value();
            }

            if (Root != nullptr) {
                auto Found = false;
                if (const auto PtrType =
                        llvm::dyn_cast<AST::PointerTypeExpr>(CurrentOper))
                {
                    PtrType->setOperand(*Expr);
                    Found = true;
                }

                if (const auto OptType =
                        llvm::dyn_cast<AST::OptionalTypeExpr>(CurrentOper))
                {
                    OptType->setOperand(*Expr);
                    Found = true;
                }

                if (const auto UnaryExpr =
                        llvm::dyn_cast<AST::UnaryOperation>(CurrentOper))
                {
                    UnaryExpr->setOperand(*Expr);
                    Found = true;
                }

                if (!Found) {
                    Diag.consume({
                        .Level = DiagnosticLevel::Error,
                        .Location = Expr->getLoc(),
                        .Message = "Expected an expression"
                    });

                    return std::unexpected(ParseError::FailedCouldNotProceed);
                }
            } else {
                Root = Expr;
            }

            if (IsUnaryOp || Token.Kind == Lex::TokenKind::Minus) {
                CurrentOper = Expr;
                continue;
            }

            return Root;
        }

        __builtin_unreachable();
    }

    auto
    ParseBinaryOperationWithRhsPrec(ParseContext &Context,
                                    AST::Expr *Lhs,
                                    const uint32_t MinPrec,
                                    const bool InPlaceOfStmt) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        do {
            auto TokenOpt = TokenStream.peek();
            if (!TokenOpt.has_value()) {
                return Lhs;
            }

            auto BinOpToken = TokenOpt.value();
            const auto BinOpTokenString = TokenStream.tokenContent(BinOpToken);

            if (!Lex::TokenKindIsBinOp(BinOpToken.Kind, BinOpTokenString)) {
                return Lhs;
            }

            const auto ThisOperInfo =
                OperatorInfoForToken(BinOpToken, BinOpTokenString);

            assert(ThisOperInfo.Precedence != Precedence::Unknown &&
                   "OperatorInfoForToken is missing a bin-op");

            if (static_cast<uint64_t>(ThisOperInfo.Precedence) < MinPrec) {
                return Lhs;
            }

            TokenStream.consume();
            if (!TokenStream.peek().has_value()) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = BinOpToken.Loc,
                    .Message =
                        std::format("Expected an expression after \"{}\"",
                                    BinOpTokenString)
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }

            const auto RhsOpt = ParseLhs(Context, InPlaceOfStmt);
            if (!RhsOpt.has_value()) {
                return std::unexpected(RhsOpt.error());
            }

            auto Rhs = RhsOpt.value();
            TokenOpt = TokenStream.peek();

            if (TokenOpt.has_value()) {
                const auto Token = TokenOpt.value();
                const auto TokenContent = TokenStream.tokenContent(Token);

                if (Lex::TokenKindIsBinOp(Token.Kind, TokenContent)) {
                    const auto ThisIsRightAssoc =
                        ThisOperInfo.Assoc == OperatorAssoc::Right;
                    const auto NextOperInfo =
                        OperatorInfoForToken(Token, TokenContent);

                    assert(NextOperInfo.Precedence != Precedence::Unknown &&
                           "OperatorInfoForToken missing entry for token");

                    if (ThisOperInfo.Precedence < NextOperInfo.Precedence ||
                        (ThisOperInfo.Precedence == NextOperInfo.Precedence &&
                         ThisIsRightAssoc))
                    {
                        const auto NextMinPrec =
                            static_cast<uint64_t>(
                                static_cast<uint64_t>(ThisOperInfo.Precedence) +
                                !ThisIsRightAssoc);

                        const auto RhsOpt =
                            ParseBinaryOperationWithRhsPrec(Context, Rhs,
                                                            InPlaceOfStmt,
                                                            NextMinPrec);

                        if (!RhsOpt.has_value()) {
                            return std::unexpected(RhsOpt.error());
                        }
                    }
                }
            }

            const auto BinOpOpt =
                LexTokenToBinaryOperator(BinOpToken, BinOpTokenString);

            assert(BinOpOpt.has_value() &&
                   "LexTokenToBinaryOperator missing BinOp");

            const auto BinOp = BinOpOpt.value();
            if (BinOp == BinaryOperator::As) {
                Lhs = new AST::CastExpr(BinOpToken.Loc, *Lhs, *Rhs);
            } else {
                Lhs =
                    new AST::BinaryOperation(BinOp, BinOpToken.Loc, *Lhs, *Rhs);
            }
        } while (true);

        __builtin_unreachable();
    }

    auto ParseExpression(ParseContext &Context) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        const auto LhsOpt = ParseLhs(Context, /*InPlaceOfStmt=*/false);
        if (!LhsOpt.has_value()) {
            return std::unexpected(LhsOpt.error());
        }

        const auto Lhs = LhsOpt.value();
        return ParseBinaryOperationWithRhsPrec(Context, Lhs, /*MinPrec=*/0,
                                               /*InPlaceOfStmt=*/false);
    }

    auto ParseExpressionInPlaceOfStmt(ParseContext &Context) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        const auto LhsOpt = ParseLhs(Context, /*InPlaceOfStmt=*/true);
        if (!LhsOpt.has_value()) {
            return std::unexpected(LhsOpt.error());
        }

        const auto Lhs = LhsOpt.value();
        return ParseBinaryOperationWithRhsPrec(Context, Lhs, /*MinPrec=*/0,
                                               /*InPlaceOfStmt=*/true);
    }
}
