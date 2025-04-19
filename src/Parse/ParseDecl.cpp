/*
 * Parse/ParseDecl.cpp
 * © suhas pai
 */

#include "AST/Decls/ArrayBindingVarDecl.h"
#include "AST/Decls/ObjectBindingVarDecl.h"

#include "AST/Decls/FieldDecl.h"
#include "AST/Decls/OptionalFieldDecl.h"
#include "AST/Decls/InterfaceDecl.h"
#include "AST/Decls/ParamVarDecl.h"
#include "AST/Decls/VarDecl.h"

#include "AST/Types/FunctionType.h"

#include "AST/CaptureAllByRefExpr.h"
#include "AST/CaptureAllByValueExpr.h"

#include "Parse/ParseDecl.h"
#include "Parse/ParseExpr.h"
#include "Parse/ParseIfExpr.h"
#include "Parse/ParseMisc.h"
#include "Parse/ParseStmt.h"

namespace Parse {
    [[nodiscard]] static bool
    VerifyDeclName(ParseContext &Context,
                   const Lex::Token NameToken,
                   const std::string_view Kind) noexcept
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        if (NameToken.Kind == Lex::TokenKind::Identifier) {
            return true;
        }

        if (NameToken.Kind == Lex::TokenKind::Keyword) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = NameToken.Loc,
                .Message =
                    std::format("Keywords cannot be used as a {} name", Kind)
            });

            return false;
        }

        const auto Name = TokenStream.tokenContent(NameToken);
        Diag.consume({
            .Level = DiagnosticLevel::Error,
            .Location = NameToken.Loc,
            .Message =
                std::format("Expected a name for {} declaration, found '{}' "
                            "instead",
                            Kind, Name)
        });

        return false;
    }

    [[nodiscard]] static auto
    ParseTypeAnnotationIfFound(ParseContext &Context,
                               const Lex::TokenKind EndToken) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        if (!TokenStream.peekIs(Lex::TokenKind::Colon)) {
            return nullptr;
        }

        const auto Colon = TokenStream.consume().value();
        const auto EndTokenList = { Lex::TokenKind::Equal, EndToken };

        if (TokenStream.peekIsOneOf(EndTokenList)) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = Colon.Loc,
                .Message = "Expected a type annotation expression after ':'"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        return ParseLhs(Context, /*InPlaceOfStmt=*/false);
    }

    [[nodiscard]]
    static auto ParseInitExpressionIfFound(ParseContext &Context) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        auto &TokenStream = Context.TokenStream;
        if (TokenStream.consumeIfIs(Lex::TokenKind::Equal)) {
            return ParseExpression(Context);
        }

        return nullptr;
    }

    [[nodiscard]] static auto
    ParseFieldDecl(ParseContext &Context,
                   const bool AllowOptionalFields) noexcept
        -> std::expected<AST::FieldDecl *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        if (TokenStream.peekIs(Lex::TokenKind::Keyword)) {
            const auto Token = TokenStream.peek().value();
            const auto TokenString = TokenStream.tokenContent(Token);

            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message =
                    std::format("Keyword \"{}\" cannot be used as a field name",
                                TokenString)
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto NameTokenOpt = TokenStream.consume();
        if (!NameTokenOpt.has_value()) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected a name for field declaration"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto NameToken = NameTokenOpt.value();
        if (!VerifyDeclName(Context, NameToken, "field")) {
            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto IsOptional =
            TokenStream.consumeIfIs(Lex::TokenKind::QuestionMark).has_value();

        const auto TypeExprOpt =
            ParseTypeAnnotationIfFound(Context,
                                       Lex::TokenKind::CloseCurlyBrace);

        if (!TypeExprOpt.has_value()) {
            // FIXME: We should skip to the next field, not the end of
            // the struct.

            return std::unexpected(TypeExprOpt.error());
        }

        const auto TypeExpr = TypeExprOpt.value();
        if (TypeExpr == nullptr) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = NameToken.Loc,
                .Message = "All fields must have a type annotation"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto InitExprOpt = ParseInitExpressionIfFound(Context);
        if (!InitExprOpt.has_value()) {
            return std::unexpected(InitExprOpt.error());
        }

        const auto Name = TokenStream.tokenContent(NameToken);
        const auto InitExpr = InitExprOpt.value();

        if (IsOptional) {
            if (!AllowOptionalFields) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = NameToken.Loc,
                    .Message = "Optional fields are not allowed"
                });
            }

            return
                new AST::OptionalFieldDecl(Name, NameToken.Loc, TypeExpr,
                                           InitExpr);
        }

        return new AST::FieldDecl(Name, NameToken.Loc, TypeExpr, InitExpr);
    }

    [[nodiscard]] static auto
    ParseFieldList(ParseContext &Context,
                   const SourceLocation DeclLoc,
                   std::vector<AST::Stmt *> &FieldList,
                   const bool AllowOptionalFields) noexcept -> ParseError
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        if (TokenStream.consumeIfIs(Lex::TokenKind::CloseCurlyBrace)) {
            return ParseError::None;
        }

        while (true) {
            auto WasControlFlowExpr = false;

            auto Stmt = static_cast<AST::Stmt *>(nullptr);
            auto CommaLocOpt = std::optional<SourceLocation>(std::nullopt);

            if (const auto IfTokenOpt =
                    TokenStream.consumeIfIsKeyword(Lex::Keyword::If))
            {
                const auto IfExprOpt =
                    ParseIfExpr(Context, IfTokenOpt.value(),
                                [=](ParseContext &Context) noexcept {
                                    return ParseFieldDecl(Context,
                                                          AllowOptionalFields);
                                },
                                /*SeparatorOpt=*/Lex::TokenKind::Comma);

                if (!IfExprOpt.has_value()) {
                    return IfExprOpt.error();
                }

                Stmt = IfExprOpt.value();
                WasControlFlowExpr = true;

                if (const auto CommaTokenOpt =
                        TokenStream.consumeIfIs(Lex::TokenKind::Comma))
                {
                    CommaLocOpt = CommaTokenOpt->Loc;
                }
            } else {
                const auto DeclOpt =
                    ParseFieldDecl(Context, AllowOptionalFields);

                if (!DeclOpt.has_value()) {
                    Diag.consume({
                        .Level = DiagnosticLevel::Error,
                        .Location = DeclLoc,
                        .Message = "Failed to parse field declaration"
                    });

                    return DeclOpt.error();
                }

                Stmt = DeclOpt.value();
            }

            FieldList.emplace_back(Stmt);
            if (TokenStream.consumeIfIs(Lex::TokenKind::CloseCurlyBrace)) {
                if (CommaLocOpt.has_value()) {
                    // Allow trailing comma before closing curly brace, but emit
                    // a warning for it.

                    Diag.consume({
                        .Level = DiagnosticLevel::Warning,
                        .Location = CommaLocOpt.value(),
                        .Message =
                            "Trailing comma before closing '}' in field list"
                    });
                }

                break;
            }

            // Allow missing commas for control-flow expressions, but not for
            // other cases.

            if (WasControlFlowExpr) {
                continue;
            }

            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected a ',' after field declaration"
            });

            return ParseError::FailedCouldNotProceed;
        }

        return ParseError::None;
    }

    [[nodiscard]] static auto
    ParseFunctionParamList(ParseContext &Context,
                           const Lex::Token ParenToken) noexcept
        -> std::expected<std::vector<AST::ParamVarDecl *>, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        const auto ProceedToNextParam = [&]() noexcept {
            return
                ProceedToAndConsumeCommaOrEnd(Context,
                                              Lex::TokenKind::CloseParen);
        };

        auto List = std::vector<AST::ParamVarDecl *>();
        if (TokenStream.consumeIfIs(Lex::TokenKind::CloseParen)) {
            return List;
        }

        do {
            const auto NameTokenOpt = TokenStream.consume();
            if (!NameTokenOpt.has_value()) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrOrPrevLoc(),
                    .Message = "Expected a name for function parameter"
                });

                switch (ProceedToNextParam()) {
                    case ProceedResult::Failed:
                        return std::unexpected(
                            ParseError::FailedCouldNotProceed);
                    case ProceedResult::Comma:
                        continue;
                    case ProceedResult::EndToken:
                        goto done;
                }

                continue;
            }

            const auto NameToken = NameTokenOpt.value();
            if (!VerifyDeclName(Context, NameToken, "parameter")) {
                switch (ProceedToNextParam()) {
                    case ProceedResult::Failed:
                        return std::unexpected(
                            ParseError::FailedCouldNotProceed);
                    case ProceedResult::Comma:
                        continue;
                    case ProceedResult::EndToken:
                        goto done;
                }

                continue;
            }

            const auto Name = TokenStream.tokenContent(NameToken);
            const auto TypeExprOpt =
                ParseTypeAnnotationIfFound(Context, Lex::TokenKind::Comma);

            if (!TypeExprOpt.has_value()) {
                return std::unexpected(TypeExprOpt.error());
            }

            const auto InitExprOpt = ParseInitExpressionIfFound(Context);
            if (!InitExprOpt.has_value()) {
                return std::unexpected(InitExprOpt.error());
            }

            const auto TypeExpr = TypeExprOpt.value();
            const auto InitExpr = InitExprOpt.value();

            List.emplace_back(
                new AST::ParamVarDecl(Name, NameToken.Loc, TypeExpr, InitExpr));

            if (TokenStream.consumeIfIs(Lex::TokenKind::CloseParen)) {
                break;
            }

            if (!TokenStream.consumeIfIs(Lex::TokenKind::Comma)) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrOrPrevLoc(),
                    .Message = "Expected ',' or ')' after function parameter"
                });

                switch (ProceedToNextParam()) {
                    case ProceedResult::Failed:
                        return std::unexpected(
                            ParseError::FailedCouldNotProceed);
                    case ProceedResult::Comma:
                        continue;
                    case ProceedResult::EndToken:
                        goto done;
                }
            }
        } while (true);

    done:
        return List;
    }

    [[nodiscard]] static
    auto ParseFunctionTypeReturnTypeIfFound(ParseContext &Context) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        if (!TokenStream.peekIs(Lex::TokenKind::ThinArrow)) {
            return nullptr;
        }

        const auto ThinArrow = TokenStream.consume().value();
        const auto EndTokenList = {
            Lex::TokenKind::Equal,
            Lex::TokenKind::OpenCurlyBrace
        };

        if (TokenStream.peekIsOneOf(EndTokenList)) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = ThinArrow.Loc,
                .Message = "Expected a return type expression after '->'"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        return ParseExpression(Context);
    }

    auto
    ParseFunctionDecl(ParseContext &Context,
                      const Lex::Token KeywordToken,
                      std::optional<Lex::Token> &NameTokOut) noexcept
        -> std::expected<AST::FunctionDecl *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto NameOrParenTokenOpt = TokenStream.consume();
        if (!NameOrParenTokenOpt.has_value()) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected a name or at least parameter list for "
                           "function declaration"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        // L-value functions (statements) must have names, but r-value function
        // (expressions) names are just ignored.

        const auto NameOrParenToken = NameOrParenTokenOpt.value();
        auto ParenToken = NameOrParenToken;

        if (NameOrParenToken.Kind != Lex::TokenKind::OpenParen) {
            if (!VerifyDeclName(Context, NameOrParenToken, "function")) {
                return std::unexpected(ParseError::FailedCouldNotProceed);
            }

            if (!TokenStream.peekIs(Lex::TokenKind::OpenParen)) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrOrPrevLoc(),
                    .Message = "Expected '{' after function declaration"
                });
            }

            NameTokOut = NameOrParenToken;
            ParenToken = TokenStream.consume().value();
        }

        auto ParamListOpt = ParseFunctionParamList(Context, ParenToken);
        if (!ParamListOpt.has_value()) {
            return std::unexpected(ParamListOpt.error());
        }

        auto CurlyTokenOpt = std::optional<Lex::Token>(std::nullopt);
        const auto ReturnTypeOpt = ParseFunctionTypeReturnTypeIfFound(Context);

        if (!ReturnTypeOpt.has_value()) {
            if (!TokenStream.proceedToAndConsume(
                    Lex::TokenKind::OpenCurlyBrace))
            {
                return std::unexpected(ParseError::FailedCouldNotProceed);
            }

            CurlyTokenOpt = TokenStream.current();
        } else {
            CurlyTokenOpt = TokenStream.consume();
            if (!CurlyTokenOpt.has_value()) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrOrPrevLoc(),
                    .Message = "Expected '{' after function declaration"
                });

                if (!TokenStream.proceedToAndConsume(
                        Lex::TokenKind::OpenCurlyBrace))
                {
                    return std::unexpected(ParseError::FailedCouldNotProceed);
                }

                CurlyTokenOpt = TokenStream.current();
            }
        }

        const auto CurlyToken = CurlyTokenOpt.value();
        if (CurlyToken.Kind != Lex::TokenKind::OpenCurlyBrace) {
            const auto CurlyTokenContent =
                TokenStream.tokenContent(CurlyToken);

            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = CurlyToken.Loc,
                .Message =
                    std::format("Expected {} after function declaration, found "
                                "'{}' instead",
                                "{",
                                CurlyTokenContent)
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto BodyOpt = ParseCompoundStmt(Context, CurlyToken);
        if (!BodyOpt.has_value()) {
            return std::unexpected(BodyOpt.error());;
        }

        auto &ParamList = ParamListOpt.value();
        return new AST::FunctionDecl(KeywordToken.Loc, std::move(ParamList),
                                     ReturnTypeOpt.value(), BodyOpt.value());
    }

    [[nodiscard]] static auto
    ParseArrowFunctionLikeDecl(ParseContext &Context,
                               AST::Expr *&ReturnTypeOut) noexcept
        -> std::expected<AST::Stmt *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        const auto ReturnTypeOpt =
            ParseTypeAnnotationIfFound(Context, Lex::TokenKind::FatArrow);

        if (!ReturnTypeOpt.has_value()) {
            return std::unexpected(ReturnTypeOpt.error());
        }

        ReturnTypeOut = ReturnTypeOpt.value();
        if (!TokenStream.consumeIfIs(Lex::TokenKind::FatArrow)) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected '=>' after inline function declaration"
            });

            // Ignore missing '=>' and assume the user just forgot to include
            // it.
        }

        // Parse the body of the arrow function-like declaration.
        //
        // This can be a compound statement, a return statement or an
        // expression. If it's an expression, it will be wrapped in a
        // return-statement for consistency with other function declarations.

        auto Body = static_cast<AST::Stmt *>(nullptr);
        if (TokenStream.peekIs(Lex::TokenKind::OpenCurlyBrace)) {
            const auto CurlyToken = TokenStream.consume().value();
            const auto Result = ParseCompoundStmt(Context, CurlyToken);

            if (!Result.has_value()) {
                return std::unexpected(Result.error());
            }

            Body = Result.value();
        } else if (
            const auto RetTokenOpt =
                TokenStream.consumeIfIsKeyword(Lex::Keyword::Return))
        {
            const auto RetToken = RetTokenOpt.value();
            Body = ParseReturnStmt(Context, RetToken).value();
        } else {
            const auto Result = ParseExpression(Context);
            if (!Result.has_value()) {
                return std::unexpected(Result.error());
            }

            Body =
                new AST::ReturnStmt(SourceLocation::invalid(), Result.value());
        }

        return Body;
    }

    [[nodiscard]] static auto
    ParseClosureCaptureList(ParseContext &Context,
                            const Lex::Token BracketToken) noexcept
        -> std::expected<std::vector<AST::Stmt *>, ParseError>
    {
        auto &TokenStream = Context.TokenStream;
        auto CaptureList = std::vector<AST::Stmt *>();

        if (TokenStream.consumeIfIs(Lex::TokenKind::RightSquareBracket)) {
            return CaptureList;
        }

        auto &Diag = Context.Diag;
        do {
            if (TokenStream.consumeIfIs(Lex::TokenKind::Ampersand)) {
                auto Pos = TokenStream.position() - 1;
                auto Qualifiers = AST::Qualifiers();

                ParseQualifiers(Context, Qualifiers);
                if (TokenStream.consumeIfIs(Lex::TokenKind::Comma)) {
                    CaptureList.emplace_back(
                        new AST::CaptureAllByRefExpr(BracketToken.Loc,
                                                     std::move(Qualifiers)));
                    continue;
                }

                if (TokenStream.consumeIfIs(Lex::TokenKind::RightSquareBracket))
                {
                    CaptureList.emplace_back(
                        new AST::CaptureAllByRefExpr(BracketToken.Loc,
                                                     std::move(Qualifiers)));
                    break;
                }

                TokenStream.goToPosition(Pos);
            }

            if (TokenStream.consumeIfIs(Lex::TokenKind::Equal)) {
                auto Pos = TokenStream.position() - 1;
                auto Qualifiers = AST::Qualifiers();

                ParseQualifiers(Context, Qualifiers);
                if (TokenStream.consumeIfIs(Lex::TokenKind::Comma)) {
                    CaptureList.emplace_back(
                        new AST::CaptureAllByValueExpr(BracketToken.Loc,
                                                       std::move(Qualifiers)));
                    continue;
                }

                if (TokenStream.consumeIfIs(Lex::TokenKind::RightSquareBracket))
                {
                    CaptureList.emplace_back(
                        new AST::CaptureAllByValueExpr(BracketToken.Loc,
                                                       std::move(Qualifiers)));
                    break;
                }

                TokenStream.goToPosition(Pos);
            }

            const auto ExprOpt = ParseExpression(Context);
            if (!ExprOpt.has_value()) {
                return std::unexpected(ExprOpt.error());
            }

            CaptureList.emplace_back(ExprOpt.value());
            if (TokenStream.consumeIfIs(Lex::TokenKind::Comma)) {
                continue;
            }

            if (TokenStream.consumeIfIs(Lex::TokenKind::RightSquareBracket)) {
                break;
            }

            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected ']'",
            });

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
        return CaptureList;
    }

    auto
    ParseClosureDecl(ParseContext &Context,
                     const Lex::Token BracketToken) noexcept
        -> std::expected<AST::ClosureDecl *, ParseError>
    {
        auto CaptureListOpt = ParseClosureCaptureList(Context, BracketToken);
        if (!CaptureListOpt.has_value()) {
            return std::unexpected(CaptureListOpt.error());
        }

        auto &TokenStream = Context.TokenStream;
        auto ParenToken = TokenStream.consume().value();

        assert(ParenToken.Kind == Lex::TokenKind::OpenParen &&
               "Token should be '(' after closure capture-list");

        auto ParamListOpt = ParseFunctionParamList(Context, ParenToken);
        if (!ParamListOpt.has_value()) {
            return std::unexpected(ParamListOpt.error());
        }

        auto ReturnTypeExpr = static_cast<AST::Expr *>(nullptr);
        const auto BodyOpt =
            ParseArrowFunctionLikeDecl(Context, ReturnTypeExpr);

        if (!BodyOpt.has_value()) {
            return std::unexpected(BodyOpt.error());
        }

        auto &CaptureList = CaptureListOpt.value();
        auto &ParamList = ParamListOpt.value();

        auto Body = BodyOpt.value();
        return new AST::ClosureDecl(BracketToken.Loc, std::move(CaptureList),
                                    std::move(ParamList), ReturnTypeExpr, Body);
    }

    auto
    ParseArrowFunctionDeclOrFunctionType(ParseContext &Context,
                                         const Lex::Token ParenToken) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        auto ParamListOpt = ParseFunctionParamList(Context, ParenToken);
        if (!ParamListOpt.has_value()) {
            return std::unexpected(ParamListOpt.error());
        }

        auto &TokenStream = Context.TokenStream;
        if (TokenStream.consumeIfIs(Lex::TokenKind::ThinArrow)) {
            const auto ReturnTypeExprOpt = ParseExpression(Context);
            if (!ReturnTypeExprOpt.has_value()) {
                return std::unexpected(ReturnTypeExprOpt.error());
            }

            auto &ParamList = ParamListOpt.value();
            const auto ReturnTypeExpr = ReturnTypeExprOpt.value();

            return new AST::FunctionTypeExpr(ParenToken.Loc,
                                             std::move(ParamList),
                                             ReturnTypeExpr);
        }

        auto ReturnTypeExpr = static_cast<AST::Expr *>(nullptr);
        const auto BodyOpt =
            ParseArrowFunctionLikeDecl(Context, ReturnTypeExpr);

        if (!BodyOpt.has_value()) {
            return std::unexpected(BodyOpt.error());
        }

        auto &ParamList = ParamListOpt.value();
        const auto Body = BodyOpt.value();

        return new AST::FunctionDecl(ParenToken.Loc, std::move(ParamList),
                                     ReturnTypeExpr, Body);
    }

    auto
    ParseInterfaceDecl(ParseContext &Context,
                       const Lex::Token InterfaceKeywordToken,
                       std::optional<Lex::Token> &NameTokenOptOut) noexcept
        -> std::expected<AST::InterfaceDecl *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto FieldList = std::vector<AST::Stmt *>();
        if (TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace)) {
            if (const auto Error =
                    ParseFieldList(Context, InterfaceKeywordToken.Loc,
                                   FieldList, /*AllowOptionalFields=*/true);
                Error != ParseError::None)
            {
                return std::unexpected(Error);
            }

            NameTokenOptOut = std::nullopt;
            return new AST::InterfaceDecl(InterfaceKeywordToken.Loc,
                                          std::move(FieldList));
        }

        const auto NameTokenOpt = TokenStream.consume();
        if (!NameTokenOpt.has_value()) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected a name for interface declaration"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto NameToken = NameTokenOpt.value();
        if (!VerifyDeclName(Context, NameToken, "interface")) {
            if (!TokenStream.proceedToAndConsume(
                    Lex::TokenKind::OpenCurlyBrace))
            {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = NameToken.Loc,
                    .Message = "Expected '{' after interface declaration"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }
        }

        NameTokenOptOut = NameToken;
        TokenStream.consume();

        if (const auto Error =
                ParseFieldList(Context, NameToken.Loc, FieldList,
                               /*AllowOptionalFields=*/true);
            Error != ParseError::None)
        {
            return std::unexpected(Error);
        }

        return new AST::InterfaceDecl(InterfaceKeywordToken.Loc,
                                      std::move(FieldList));
    }

    auto
    ParseShapeDecl(ParseContext &Context,
                   const Lex::Token ShapeKeywordToken,
                   std::optional<Lex::Token> &NameTokenOptOut) noexcept
        -> std::expected<AST::ShapeDecl *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto FieldList = std::vector<AST::Stmt *>();
        if (TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace)) {
            if (const auto Error =
                    ParseFieldList(Context, ShapeKeywordToken.Loc, FieldList,
                                   /*AllowOptionalFields=*/true);
                Error != ParseError::None)
            {
                return std::unexpected(Error);
            }

            NameTokenOptOut = std::nullopt;
            return new AST::ShapeDecl(ShapeKeywordToken.Loc,
                                      std::move(FieldList));
        }

        const auto NameTokenOpt = TokenStream.consume();
        if (!NameTokenOpt.has_value()) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected a name for struct declaration"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto NameToken = NameTokenOpt.value();
        if (!VerifyDeclName(Context, NameToken, "shape")) {
            if (!TokenStream.proceedToAndConsume(
                    Lex::TokenKind::OpenCurlyBrace))
            {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = NameToken.Loc,
                    .Message = "Expected '{' after shape declaration"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }
        }

        NameTokenOptOut = NameToken;
        TokenStream.consume();

        if (const auto Error =
                ParseFieldList(Context, NameToken.Loc, FieldList,
                               /*AllowOptionalFields=*/true);
            Error != ParseError::None)
        {
            return std::unexpected(Error);
        }

        return new AST::ShapeDecl(NameToken.Loc, std::move(FieldList));
    }

    auto
    ParseStructDecl(ParseContext &Context,
                    const Lex::Token StructKeywordToken,
                    std::optional<Lex::Token> &NameTokenOptOut) noexcept
        -> std::expected<AST::StructDecl *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto FieldList = std::vector<AST::Stmt *>();
        const auto StructKeywordTokenOpt = TokenStream.current();

        if (TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace)) {
            const auto StructKeywordToken = StructKeywordTokenOpt.value();
            if (const auto Error =
                    ParseFieldList(Context, StructKeywordToken.Loc, FieldList,
                                   /*AllowOptionalFields=*/false);
                Error != ParseError::None)
            {
                return std::unexpected(Error);
            }

            NameTokenOptOut = std::nullopt;
            return new AST::StructDecl(StructKeywordToken.Loc,
                                       std::move(FieldList));
        }

        const auto NameTokenOpt = TokenStream.consume();
        if (!NameTokenOpt.has_value()) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected a name for struct declaration"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto NameToken = NameTokenOpt.value();
        if (!VerifyDeclName(Context, NameToken, "struct")) {
            if (!TokenStream.proceedToAndConsume(
                    Lex::TokenKind::OpenCurlyBrace))
            {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = NameToken.Loc,
                    .Message = "Expected '{' after struct declaration"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }
        }

        NameTokenOptOut = NameToken;
        TokenStream.consume();

        if (const auto Error =
                ParseFieldList(Context, NameToken.Loc, FieldList,
                               /*AllowOptionalFields=*/false);
            Error != ParseError::None)
        {
            return std::unexpected(Error);
        }

        return new AST::StructDecl(StructKeywordToken.Loc,
                                   std::move(FieldList));
    }

    auto
    ParseUnionDecl(ParseContext &Context,
                   const Lex::Token UnionKeywordToken,
                   std::optional<Lex::Token> &NameTokenOptOut) noexcept
        -> std::expected<AST::UnionDecl *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto FieldList = std::vector<AST::Stmt *>();
        const auto UnionKeywordTokenOpt = TokenStream.current();

        if (TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace)) {
            const auto UnionKeywordToken = UnionKeywordTokenOpt.value();
            if (const auto Error =
                    ParseFieldList(Context, UnionKeywordToken.Loc, FieldList,
                                   /*AllowOptionalFields=*/false);
                Error != ParseError::None)
            {
                return std::unexpected(Error);
            }

            NameTokenOptOut = std::nullopt;
            return new AST::UnionDecl(UnionKeywordToken.Loc,
                                      std::move(FieldList));
        }

        const auto NameTokenOpt = TokenStream.consume();
        if (!NameTokenOpt.has_value()) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected a name for struct declaration"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto NameToken = NameTokenOpt.value();
        if (!VerifyDeclName(Context, NameToken, "union")) {
            if (!TokenStream.proceedToAndConsume(
                    Lex::TokenKind::OpenCurlyBrace))
            {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = NameToken.Loc,
                    .Message = "Expected '{' after union declaration"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }
        }

        NameTokenOptOut = NameToken;
        TokenStream.consume();

        if (const auto Error =
                ParseFieldList(Context, NameToken.Loc, FieldList,
                               /*AllowOptionalFields=*/false);
            Error != ParseError::None)
        {
            return std::unexpected(Error);
        }

        return new AST::UnionDecl(UnionKeywordToken.Loc, std::move(FieldList));
    }

    [[nodiscard]] static auto
    ParseArrayBindingItemList(ParseContext &Context) noexcept
        -> std::expected<std::vector<AST::ArrayBindingItem *>, ParseError>;

    [[nodiscard]] static auto
    ParseObjectBindingFieldList(ParseContext &Context) noexcept
        -> std::expected<std::vector<AST::ObjectBindingField *>,
                         ParseError>;

    [[nodiscard]] static auto
    ParseRightSideOfArrayBindingItemColon(
        ParseContext &Context,
        AST::Expr *const IndexExpr,
        const SourceLocation IndexLoc) noexcept
            -> std::expected<AST::ArrayBindingItem *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto Qualifiers = AST::Qualifiers();
        ParseQualifiers(Context, Qualifiers);

        if (TokenStream.consumeIfIs(Lex::TokenKind::LeftSquareBracket)) {
            auto ItemLoc = TokenStream.getCurrOrPrevLoc();
            auto ArrayBindingItemListOpt = ParseArrayBindingItemList(Context);

            if (!ArrayBindingItemListOpt.has_value()) {
                return std::unexpected(ArrayBindingItemListOpt.error());
            }

            auto &ArrayBindingItemList = ArrayBindingItemListOpt.value();
            auto IndexItem =
                std::optional<AST::ArrayBindingIndex>(std::nullopt);

            if (IndexExpr != nullptr) {
                IndexItem.emplace(IndexExpr, IndexLoc);
            }

            const auto Result =
                new AST::ArrayBindingItemArray(Qualifiers, IndexItem,
                                               std::move(ArrayBindingItemList),
                                               ItemLoc);

            return Result;
        }

        if (TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace)) {
            auto ItemLoc = TokenStream.getCurrOrPrevLoc();
            auto ObjectBindingListOpt = ParseObjectBindingFieldList(Context);

            if (!ObjectBindingListOpt.has_value()) {
                return std::unexpected(ObjectBindingListOpt.error());
            }

            auto &ObjectBindingList = ObjectBindingListOpt.value();
            auto IndexItem =
                std::optional<AST::ArrayBindingIndex>(std::nullopt);

            if (IndexExpr != nullptr) {
                IndexItem.emplace(IndexExpr, IndexLoc);
            }

            const auto Result =
                new AST::ArrayBindingItemObject(Qualifiers, IndexItem,
                                                std::move(ObjectBindingList),
                                                ItemLoc);

            return Result;
        }

        const auto NameTokenOpt = TokenStream.consume();
        if (!NameTokenOpt.has_value()) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected a name expression for array-like "
                           "destructuring"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto NameToken = NameTokenOpt.value();
        const auto Name = TokenStream.tokenContent(NameToken);

        if (!VerifyDeclName(Context, NameToken, "variable")) {
            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        if (IndexExpr == nullptr) {
            return
                new AST::ArrayBindingItemIdentifier(Qualifiers,
                                                    /*Index=*/std::nullopt,
                                                    Name, NameToken.Loc);
        }

        const auto IndexItem = AST::ArrayBindingIndex(IndexExpr, IndexLoc);
        const auto Result =
            new AST::ArrayBindingItemIdentifier(Qualifiers, IndexItem, Name,
                                                NameToken.Loc);

        return Result;
    }

    [[nodiscard]]
    static auto ParseArrayBindingItemList(ParseContext &Context) noexcept
        -> std::expected<std::vector<AST::ArrayBindingItem *>, ParseError>;

    [[nodiscard]] static auto
    ParseSingleArrayBindingItem(ParseContext &Context) noexcept
        -> std::expected<AST::ArrayBindingItem *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        // Each array-destructure item can be one of the following:
        //  (1) A single identifier name i.e. `[ nameS ]`
        //  (2) A number literal followed by a colon i.e. `[ 0 : nameS ]`
        //  (3) A spread operator i.e. `[ ...rest ]`
        //
        // Note that `name` can be either an identifier, or an object
        // destructuring expression. This is handled in
        // `ParseRightSideOfArrayBindingItemColon`.

        // Quick check to see if we just have a name, and not an index
        // expression

        auto Qualifiers = AST::Qualifiers();
        ParseQualifiers(Context, Qualifiers);

        if (TokenStream.consumeIfIs(Lex::TokenKind::Identifier)) {
            if (TokenStream.peekIs(Lex::TokenKind::Comma) ||
                TokenStream.peekIs(Lex::TokenKind::RightSquareBracket))
            {
                TokenStream.goBack();

                const auto IndexLoc = SourceLocation::invalid();
                return ParseRightSideOfArrayBindingItemColon(
                    Context, /*IndexExpr=*/nullptr, IndexLoc);
            }

            TokenStream.goBack();
        }

        if (TokenStream.consumeIfIs(Lex::TokenKind::LeftSquareBracket)) {
            auto ItemLoc = TokenStream.getCurrOrPrevLoc();
            auto ItemListOpt = ParseArrayBindingItemList(Context);

            if (!ItemListOpt.has_value()) {
                return std::unexpected(ItemListOpt.error());
            }

            auto &ItemList = ItemListOpt.value();
            return new AST::ArrayBindingItemArray(Qualifiers,
                                                  /*Index=*/std::nullopt,
                                                  std::move(ItemList), ItemLoc);
        }

        if (TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace)) {
            auto ItemLoc = TokenStream.getCurrOrPrevLoc();
            auto ItemListOpt = ParseObjectBindingFieldList(Context);

            if (!ItemListOpt.has_value()) {
                return std::unexpected(ItemListOpt.error());
            }

            auto &ItemList = ItemListOpt.value();
            return new AST::ArrayBindingItemObject(Qualifiers,
                                                   /*Index=*/std::nullopt,
                                                   std::move(ItemList),
                                                   ItemLoc);
        }

        if (TokenStream.consumeIfIs(Lex::TokenKind::DotDotDot)) {
            const auto ItemLoc = TokenStream.getCurrOrPrevLoc();
            const auto NameTokenOpt = TokenStream.consume();

            if (!NameTokenOpt.has_value()) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrOrPrevLoc(),
                    .Message = "Expected a name expression for spread operator"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }

            const auto NameToken = NameTokenOpt.value();
            if (NameToken.Kind == Lex::TokenKind::RightSquareBracket) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrOrPrevLoc(),
                    .Message = "Spread operator missing a name"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }

            if (!VerifyDeclName(Context, NameToken, "variable")) {
                return std::unexpected(ParseError::FailedCouldNotProceed);
            }

            const auto Name = TokenStream.tokenContent(NameToken);
            const auto Result =
                new AST::ArrayBindingItemSpread(Qualifiers,
                                                /*Index=*/std::nullopt,
                                                Name, NameToken.Loc, ItemLoc);

            return Result;
        }

        const auto IndexLoc = TokenStream.getCurrOrPrevLoc();
        const auto IndexExprOpt = ParseExpression(Context);

        if (IndexExprOpt.has_value()) {
            return std::unexpected(IndexExprOpt.error());
        }

        if (TokenStream.consumeIfIs(Lex::TokenKind::Colon)) {
            const auto IndexExpr = IndexExprOpt.value();
            const auto BindingItem =
                ParseRightSideOfArrayBindingItemColon(Context, IndexExpr,
                                                      IndexLoc);

            return BindingItem;
        }

        // FIXME: We're missing a colon here, recover here.
        return std::unexpected(ParseError::FailedCouldNotProceed);
    }

    [[nodiscard]]
    static auto ParseArrayBindingItemList(ParseContext &Context) noexcept
        -> std::expected<std::vector<AST::ArrayBindingItem *>, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto BindingItemList = std::vector<AST::ArrayBindingItem *>();
        do {
            const auto BindingItemOpt = ParseSingleArrayBindingItem(Context);
            if (!BindingItemOpt.has_value()) {
                return std::unexpected(BindingItemOpt.error());
            }

            BindingItemList.emplace_back(BindingItemOpt.value());
            if (TokenStream.consumeIfIs(Lex::TokenKind::RightSquareBracket)) {
                break;
            }

            if (TokenStream.consumeIfIs(Lex::TokenKind::Comma)) {
                continue;
            }

            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected ',' or ']' after array-like "
                            "destructuring item"
            });

            const auto ProceedResult =
                ProceedToAndConsumeCommaOrEnd(
                    Context, Lex::TokenKind::RightSquareBracket);

            switch (ProceedResult) {
                case ProceedResult::Failed:
                    Diag.consume({
                        .Level = DiagnosticLevel::Error,
                        .Location = TokenStream.getCurrOrPrevLoc(),
                        .Message = "Reached end of file while parsing "
                                   "array-like destructuring, expected ']'"
                    });

                    return std::unexpected(ParseError::FailedCouldNotProceed);
                case ProceedResult::Comma:
                    continue;
                case ProceedResult::EndToken:
                    goto done;
            }
        } while (true);

    done:
        return BindingItemList;
    }

    [[nodiscard]] static auto
    ParseArrayBindingVarDecl(ParseContext &Context,
                             const AST::Qualifiers &PreIntroducerQualifiers,
                             const SourceLocation SquareBracketLoc) noexcept
        -> std::expected<AST::Stmt *, ParseError>
    {
        auto Qualifiers = AST::Qualifiers();
        ParseQualifiers(Context, Qualifiers);

        auto BindingItemListOpt = ParseArrayBindingItemList(Context);
        if (!BindingItemListOpt.has_value()) {
            return std::unexpected(BindingItemListOpt.error());
        }

        const auto InitExprOpt = ParseInitExpressionIfFound(Context);
        if (!InitExprOpt.has_value()) {
            return std::unexpected(InitExprOpt.error());
        }

        auto &BindingItemList = BindingItemListOpt.value();
        return new AST::ArrayBindingVarDecl(SquareBracketLoc, Qualifiers,
                                            std::move(BindingItemList),
                                            InitExprOpt.value());
    }

    [[nodiscard]]
    static auto ParseSingleObjectBindingField(ParseContext &Context) noexcept
        -> std::expected<AST::ObjectBindingField *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto AtKeyLocQualifiers = AST::Qualifiers();
        ParseQualifiers(Context, AtKeyLocQualifiers);

        const auto KeyTokenOpt = TokenStream.consume();
        if (!KeyTokenOpt.has_value()) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected a name for object-like destructuring"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto KeyToken = KeyTokenOpt.value();
        if (KeyToken.Kind == Lex::TokenKind::DotDotDot) {
            const auto NameTokenOpt = TokenStream.consume();
            if (!NameTokenOpt.has_value()) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = KeyToken.Loc,
                    .Message = "Expected a variable name to use spread operator"
                });

                return std::unexpected(ParseError::FailedCouldNotProceed);
            }

            const auto NameToken = NameTokenOpt.value();
            if (!VerifyDeclName(Context, NameToken, "variable")) {
                return std::unexpected(ParseError::FailedCouldNotProceed);
            }

            const auto Name = TokenStream.tokenContent(NameToken);
            const auto Result =
                new AST::ObjectBindingFieldSpread(Name, NameToken.Loc,
                                                  KeyToken.Loc,
                                                  AtKeyLocQualifiers);

            return Result;
        }

        if (!VerifyDeclName(Context, KeyToken, "field")) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = KeyToken.Loc,
                .Message = "Expected a name for object-like destructuring"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto KeyLoc = KeyToken.Loc;

        // Each object-destructure item can take one of the following forms;
        //  (1) The key, followed by a colon and name
        //      i.e `{ keyS: qual nameS }`
        //
        //  (2) The key and nothing else, (i.e `{ [qual] keyS }`), which is the
        //      same as `{ keyS: qual keyS }`
        //
        //  (3) The key, followed by a colon and an array destructure
        //      i.e `{ keyS: qual [nameS] }`

        auto NameLoc = KeyToken.Loc;
        auto NameToken = KeyToken;

        if (!TokenStream.consumeIfIs(Lex::TokenKind::Colon)) {
            const auto Key = TokenStream.tokenContent(KeyToken);
            const auto Name = TokenStream.tokenContent(NameToken);

            return new AST::ObjectBindingFieldIdentifier(
                Key, KeyLoc, Name, NameLoc, AtKeyLocQualifiers);
        }

        // FIXME: Do something with the previous qualifier values.

        AtKeyLocQualifiers.clear();
        ParseQualifiers(Context, AtKeyLocQualifiers);

        const auto Key = TokenStream.tokenContent(KeyToken);
        if (TokenStream.consumeIfIs(Lex::TokenKind::LeftSquareBracket)) {
            auto ArrayBindingItemListOpt = ParseArrayBindingItemList(Context);
            if (!ArrayBindingItemListOpt.has_value()) {
                // FIXME: Properly recover here.
                return std::unexpected(ArrayBindingItemListOpt.error());
            }

            auto &ArrayBindingItemList = ArrayBindingItemListOpt.value();
            return new AST::ObjectBindingFieldArray(
                Key, KeyLoc, AtKeyLocQualifiers,
                std::move(ArrayBindingItemList));
        }

        if (TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace)) {
            auto ObjectBindingFieldListOpt =
                ParseObjectBindingFieldList(Context);

            if (!ObjectBindingFieldListOpt.has_value()) {
                // FIXME: Properly recover here.
                return std::unexpected(ObjectBindingFieldListOpt.error());
            }

            auto &ObjectBindingFieldList = ObjectBindingFieldListOpt.value();
            return new AST::ObjectBindingFieldObject(
                Key, KeyLoc, AtKeyLocQualifiers,
                std::move(ObjectBindingFieldList));
        }

        const auto NameTokenOpt = TokenStream.consume();
        if (!NameTokenOpt.has_value()) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected a name for object-like destructuring"
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        NameToken = NameTokenOpt.value();
        if (NameToken.Kind != Lex::TokenKind::Identifier) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = NameToken.Loc,
                .Message =
                    std::format("Expected a name for object-like "
                                "destructuring, got \"{}\" instead",
                                TokenStream.tokenContent(NameToken))
            });

            return std::unexpected(ParseError::FailedCouldNotProceed);
        }

        const auto Name = TokenStream.tokenContent(NameToken);
        return new AST::ObjectBindingFieldIdentifier(Key, KeyLoc, Name,
                                                     NameToken.Loc,
                                                     AtKeyLocQualifiers);
    }

    [[nodiscard]]
    static auto ParseObjectBindingFieldList(ParseContext &Context) noexcept
        -> std::expected<std::vector<AST::ObjectBindingField *>, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto BindingItemList = std::vector<AST::ObjectBindingField *>();
        if (TokenStream.consumeIfIs(Lex::TokenKind::CloseCurlyBrace)) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Can't have an empty object-like destructure-list "
                           "after '{'"
            });

            return std::unexpected(ParseError::FailedAndProceeded);
        }

        do {
            const auto BindingItemOpt = ParseSingleObjectBindingField(Context);
            if (!BindingItemOpt.has_value()) {
                return std::unexpected(BindingItemOpt.error());
            }

            BindingItemList.emplace_back(BindingItemOpt.value());
            if (TokenStream.consumeIfIs(Lex::TokenKind::CloseCurlyBrace)) {
                break;
            }

            if (TokenStream.consumeIfIs(Lex::TokenKind::Comma)) {
                continue;
            }

            const auto ProceedResult =
                ProceedToAndConsumeCommaOrEnd(Context,
                                              Lex::TokenKind::CloseCurlyBrace);

            switch (ProceedResult) {
                case ProceedResult::Failed:
                    Diag.consume({
                        .Level = DiagnosticLevel::Error,
                        .Location = TokenStream.getCurrOrPrevLoc(),
                        .Message = "Expected ',' or '}' after object-like "
                                   "destructuring item"
                    });

                    return std::unexpected(ParseError::FailedCouldNotProceed);
                case ProceedResult::Comma:
                    continue;
                case ProceedResult::EndToken:
                    goto done;
            }
        } while (true);

    done:
        return BindingItemList;
    }

    [[nodiscard]] static auto
    ParseObjectBindingVarDecl(ParseContext &Context,
                              const AST::Qualifiers &PreQualifiers,
                              const SourceLocation CurlyLoc) noexcept
        -> std::expected<AST::Stmt *, ParseError>
    {
        auto Qualifiers = AST::Qualifiers();
        ParseQualifiers(Context, Qualifiers);

        auto BindingItemListOpt = ParseObjectBindingFieldList(Context);
        if (!BindingItemListOpt.has_value()) {
            return std::unexpected(BindingItemListOpt.error());
        }

        auto &BindingItemList = BindingItemListOpt.value();
        const auto InitExprOpt = ParseInitExpressionIfFound(Context);

        if (!InitExprOpt.has_value()) {
            return std::unexpected(InitExprOpt.error());
        }

        const auto InitExpr = InitExprOpt.value();
        return new AST::ObjectBindingVarDecl(CurlyLoc, Qualifiers,
                                             std::move(BindingItemList),
                                             InitExpr);
    }

    auto
    ParseVarDecl(ParseContext &Context,
                 const AST::Qualifiers &PreQualifiers) noexcept
        -> std::expected<AST::Stmt *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto DeclLoc = SourceLocation();
        auto Qualifiers = AST::Qualifiers();

        ParseQualifiers(Context, Qualifiers);

        const auto NameTokenOpt = TokenStream.consume();
        if (NameTokenOpt.has_value()) {
            const auto NameToken = NameTokenOpt.value();
            if (NameToken.Kind == Lex::TokenKind::LeftSquareBracket) {
                return ParseArrayBindingVarDecl(Context, PreQualifiers,
                                                NameToken.Loc);
            }

            if (NameToken.Kind == Lex::TokenKind::OpenCurlyBrace) {
                return ParseObjectBindingVarDecl(Context, PreQualifiers,
                                                 NameToken.Loc);
            }

            if (!VerifyDeclName(Context, NameToken, "variable")) {
                return std::unexpected(ParseError::FailedCouldNotProceed);
            }

            DeclLoc = NameTokenOpt.value().Loc;
        } else {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected a name for variable declaration"
            });

            // FIXME:
            DeclLoc = TokenStream.getCurrOrPrevLoc();
        }

        const auto TypeExprOpt =
            ParseTypeAnnotationIfFound(Context, Lex::TokenKind::Semicolon);

        if (!TypeExprOpt.has_value()) {
            return std::unexpected(TypeExprOpt.error());
        }

        const auto TypeExpr = TypeExprOpt.value();
        const auto NameLoc =
            NameTokenOpt
                .transform([&](const auto &Token) noexcept {
                    return Token.Loc;
                })
                .value_or(DeclLoc);

        const auto Name =
            NameTokenOpt
                .transform([&](const auto &Token) noexcept {
                    return TokenStream.tokenContent(Token);
                })
                .value_or(std::string_view());

        const auto InitExprOpt = ParseInitExpressionIfFound(Context);
        if (!InitExprOpt.has_value()) {
            return std::unexpected(InitExprOpt.error());
        }

        const auto InitExpr = InitExprOpt.value();
        if (InitExpr == nullptr) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Variable declarations must have an initial value"
            });
        }

        return new AST::VarDecl(Name, NameLoc, Qualifiers, TypeExpr, InitExpr);
    }
}
