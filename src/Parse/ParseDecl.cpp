/*
 * Parse/ParseVarDecl.cpp
 * © suhas pai
 */

#include "AST/Decls/ArrayDestructredVarDecl.h"
#include "AST/Decls/ObjectDestructuredVarDecl.h"

#include "AST/Decls/FieldDecl.h"
#include "AST/Decls/ParamVarDecl.h"
#include "AST/Decls/VarDecl.h"

#include "AST/Types/FunctionType.h"
#include "AST/NumberLiteral.h"

#include "Parse/ParseDecl.h"
#include "Parse/ParseExpr.h"
#include "Parse/ParseMisc.h"
#include "Parse/ParseStmt.h"

#include "llvm/Support/Casting.h"

namespace Parse {
    [[nodiscard]]
    static inline auto ParseTypeExpr(ParseContext &Context) noexcept {
        return ParseExpression(Context);
    }

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
                               const SourceLocation Loc,
                               const Lex::TokenKind EndToken) noexcept
        -> AST::Expr *
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

            return nullptr;
        }

        return ParseTypeExpr(Context);
    }

    auto
    ParseFieldList(ParseContext &Context,
                   const SourceLocation StructDeclLoc,
                   std::vector<AST::Stmt *> &FieldList) noexcept -> bool
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        if (TokenStream.consumeIfIs(Lex::TokenKind::CloseCurlyBrace)) {
            return true;
        }

        while (true) {
            if (TokenStream.peekIs(Lex::TokenKind::Keyword)) {
                const auto Token = TokenStream.peek().value();
                const auto TokenString = TokenStream.tokenContent(Token);

                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrOrPrevLoc(),
                    .Message =
                        std::format("Keyword \"{}\" cannot be used as a field "
                                    "name",
                                    TokenString)
                });

                return false;
            }

            const auto NameTokenOpt = TokenStream.consume();
            if (!NameTokenOpt.has_value()) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrOrPrevLoc(),
                    .Message = "Expected a name for field declaration"
                });

                return false;
            }

            const auto NameToken = NameTokenOpt.value();
            if (!VerifyDeclName(Context, NameToken, "field")) {
                return false;
            }

            const auto TypeExpr =
                ParseTypeAnnotationIfFound(Context, NameToken.Loc,
                                           Lex::TokenKind::CloseCurlyBrace);

            if (TypeExpr == nullptr) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = NameToken.Loc,
                    .Message = "All fields must have a type annotation"
                });

                return false;
            }

            auto InitExpr = static_cast<AST::Expr *>(nullptr);
            if (TokenStream.consumeIfIs(Lex::TokenKind::Equal)) {
                InitExpr = ParseExpression(Context);
                if (InitExpr == nullptr) {
                    return false;
                }
            }

            const auto Name = TokenStream.tokenContent(NameToken);
            FieldList.emplace_back(
                new AST::FieldDecl(Name, NameToken.Loc, TypeExpr, InitExpr));

            if (TokenStream.consumeIfIs(Lex::TokenKind::CloseCurlyBrace)) {
                break;
            }

            if (TokenStream.consumeIfIs(Lex::TokenKind::Comma)) {
                continue;
            }
        }

        return true;
    }

    static auto
    ParseInitExpressionIfFound(ParseContext &Context,
                               const Lex::TokenKind EndToken) noexcept
        -> AST::Expr *
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        if (!TokenStream.peekIs(Lex::TokenKind::Equal)) {
            return nullptr;
        }

        const auto EqualSignTok = TokenStream.consume().value();
        if (TokenStream.peekIs(EndToken)) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = EqualSignTok.Loc,
                .Message = "Expected a initial value expression after '='"
            });

            return nullptr;
        }

        return ParseExpression(Context);
    }

    static auto
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
            const auto TypeExpr =
                ParseTypeAnnotationIfFound(Context, NameToken.Loc,
                                           Lex::TokenKind::Comma);

            const auto InitExpr =
                ParseInitExpressionIfFound(Context, Lex::TokenKind::Comma);

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

    [[nodiscard]]
    static auto ParseFunctionReturnTypeIfFound(ParseContext &Context) noexcept
        -> std::optional<AST::Expr *>
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

            return std::nullopt;
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

            return nullptr;
        }

        // L-value functions (statements) must have names, but r-value function
        // (expressions) names are just ignored.

        const auto NameOrParenToken = NameOrParenTokenOpt.value();
        auto ParenToken = NameOrParenToken;

        if (NameOrParenToken.Kind != Lex::TokenKind::OpenParen) {
            if (!VerifyDeclName(Context, NameOrParenToken, "function")) {
                return nullptr;
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
        const auto ReturnTypeOpt = ParseFunctionReturnTypeIfFound(Context);

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
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = CurlyToken.Loc,
                .Message =
                    std::format("Expected {} after function declaration, found "
                                "'{}' instead",
                                "{",
                                TokenStream.tokenContent(CurlyToken))
            });

            return nullptr;
        }

        const auto Body = ParseCompoundStmt(Context, CurlyToken);
        if (Body == nullptr) {
            return nullptr;
        }

        auto &ParamList = ParamListOpt.value();
        return new AST::FunctionDecl(KeywordToken.Loc, std::move(ParamList),
                                     ReturnTypeOpt.value(), Body);
    }

    static auto
    ParseArrowFunctionLikeDecl(ParseContext &Context,
                               AST::Expr *&ReturnTypeExpr,
                               AST::Stmt *&Body) noexcept -> bool
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        if (TokenStream.consumeIfIs(Lex::TokenKind::Colon)) {
            ReturnTypeExpr = ParseExpression(Context);
            if (ReturnTypeExpr == nullptr) {
                return false;
            }
        }

        if (!TokenStream.consumeIfIs(Lex::TokenKind::FatArrow)) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected '=>' after inline function declaration"
            });
        }

        if (TokenStream.peekIs(Lex::TokenKind::OpenCurlyBrace)) {
            const auto CurlyToken = TokenStream.consume().value();

            Body = ParseCompoundStmt(Context, CurlyToken);
            if (Body == nullptr) {
                return false;
            }
        } else if (TokenStream.peekIs(Lex::TokenKind::Keyword)) {
            const auto Token = TokenStream.peek().value();
            const auto Keyword =
                Lex::KeywordTokenGetKeyword(TokenStream.tokenContent(Token));

            if (Keyword == Lex::Keyword::Return) {
                TokenStream.consume();
                Body = ParseReturnStmt(Context, Token).value();

                if (Body == nullptr) {
                    return false;
                }
            }

            // Unexpected token, pass error-handling to ParseExpression()
            // instead of handling it here.

            Body = ParseExpression(Context);
            if (Body == nullptr) {
                return false;
            }
        } else {
            Body = ParseExpression(Context);
            if (Body == nullptr) {
                return false;
            }
        }

        // For arrow functions, wrap expressions in a return-statement
        if (const auto BodyExpr = llvm::dyn_cast<AST::Expr>(Body)) {
            Body = new AST::ReturnStmt(SourceLocation::invalid(), BodyExpr);
        }

        return true;
    }

    auto
    ParseClosureDecl(ParseContext &Context,
                     const Lex::Token ParenToken,
                     std::vector<AST::Stmt *> &&CaptureList) noexcept
        -> std::expected<AST::ClosureDecl *, ParseError>
    {
        auto ParamListOpt = ParseFunctionParamList(Context, ParenToken);
        if (!ParamListOpt.has_value()) {
            return std::unexpected(ParamListOpt.error());
        }

        auto ReturnTypeExpr = static_cast<AST::Expr *>(nullptr);
        auto Body = static_cast<AST::Stmt *>(nullptr);

        if (!ParseArrowFunctionLikeDecl(Context, ReturnTypeExpr, Body)) {
            return nullptr;
        }

        auto &ParamList = ParamListOpt.value();
        return new AST::ClosureDecl(ParenToken.Loc, std::move(CaptureList),
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
            const auto ReturnTypeExpr = ParseTypeExpr(Context);
            if (ReturnTypeExpr == nullptr) {
                return nullptr;
            }

            auto &ParamList = ParamListOpt.value();
            return new AST::FunctionTypeExpr(ParenToken.Loc,
                                             std::move(ParamList),
                                             ReturnTypeExpr);
        }

        auto ReturnTypeExpr = static_cast<AST::Expr *>(nullptr);
        auto Body = static_cast<AST::Stmt *>(nullptr);

        if (!ParseArrowFunctionLikeDecl(Context, ReturnTypeExpr, Body)) {
            return nullptr;
        }

        // Function Decl.
        auto &ParamList = ParamListOpt.value();
        return new AST::FunctionDecl(ParenToken.Loc, std::move(ParamList),
                                     ReturnTypeExpr, Body);
    }

    auto
    ParseStructDecl(ParseContext &Context,
                    const Lex::Token StructKeywordToken,
                    const bool IsLValue,
                    std::optional<Lex::Token> &NameTokenOptOut) noexcept
        -> std::expected<AST::StructDecl *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto FieldList = std::vector<AST::Stmt *>();
        const auto StructKeywordTokenOpt = TokenStream.current();

        if (TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace)) {
            const auto StructKeywordToken = StructKeywordTokenOpt.value();
            if (!ParseFieldList(Context, StructKeywordToken.Loc, FieldList)) {
                return nullptr;
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

            return nullptr;
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

                return nullptr;
            }
        }

        NameTokenOptOut = NameToken;
        TokenStream.consume();

        if (!ParseFieldList(Context, NameToken.Loc, FieldList)) {
            return nullptr;
        }

        return new AST::StructDecl(StructKeywordToken.Loc,
                                   std::move(FieldList));
    }

    [[nodiscard]] static auto
    ParseArrayDestructureItemList(ParseContext &Context) noexcept
        -> std::expected<std::vector<AST::ArrayDestructureItem *>, ParseError>;

    [[nodiscard]] static auto
    ParseObjectDestructureFieldList(ParseContext &Context) noexcept
        -> std::expected<std::vector<AST::ObjectDestructureField *>,
                         ParseError>;

    [[nodiscard]] static auto
    ParseRightSideOfArrayDestructureItemColon(
        ParseContext &Context,
        AST::Expr *const IndexExpr,
        const SourceLocation IndexLoc) noexcept -> AST::ArrayDestructureItem *
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto Qualifiers = AST::Qualifiers();
        ParseQualifiers(Context, Qualifiers);

        if (TokenStream.consumeIfIs(Lex::TokenKind::LeftSquareBracket)) {
            auto ItemLoc = TokenStream.getCurrOrPrevLoc();
            auto ArrayDestructureItemListOpt =
                ParseArrayDestructureItemList(Context);

            if (!ArrayDestructureItemListOpt.has_value()) {
                return nullptr;
            }

            auto &ArrayDestructureItemList =
                ArrayDestructureItemListOpt.value();

            auto IndexItem =
                std::optional<AST::ArrayDestructureIndex>(std::nullopt);

            if (IndexExpr != nullptr) {
                IndexItem.emplace(IndexExpr, IndexLoc);
            }

            const auto Result =
                new AST::ArrayDestructureItemArray(
                    Qualifiers, IndexItem, std::move(ArrayDestructureItemList),
                    ItemLoc);

            return Result;
        }

        if (TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace)) {
            auto ItemLoc = TokenStream.getCurrOrPrevLoc();
            auto ObjectDestructureListOpt =
                ParseObjectDestructureFieldList(Context);

            if (!ObjectDestructureListOpt.has_value()) {
                return nullptr;
            }

            auto &ObjectDestructureList = ObjectDestructureListOpt.value();
            auto IndexItem =
                std::optional<AST::ArrayDestructureIndex>(std::nullopt);

            if (IndexExpr != nullptr) {
                IndexItem.emplace(IndexExpr, IndexLoc);
            }

            const auto Result =
                new AST::ArrayDestructureItemObject(
                    Qualifiers, IndexItem, std::move(ObjectDestructureList),
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

            return nullptr;
        }

        const auto NameToken = NameTokenOpt.value();
        const auto Name = TokenStream.tokenContent(NameToken);

        if (!VerifyDeclName(Context, NameToken, "variable")) {
            return nullptr;
        }

        if (IndexExpr == nullptr) {
            return
                new AST::ArrayDestructureItemIdentifier(Qualifiers,
                                                        /*Index=*/std::nullopt,
                                                        Name, NameToken.Loc);
        }

        const auto IndexItem = AST::ArrayDestructureIndex(IndexExpr, IndexLoc);
        const auto Result =
            new AST::ArrayDestructureItemIdentifier(Qualifiers, IndexItem, Name,
                                                    NameToken.Loc);

        return Result;
    }

    [[nodiscard]]
    static auto ParseArrayDestructureItemList(ParseContext &Context) noexcept
        -> std::expected<std::vector<AST::ArrayDestructureItem *>, ParseError>;

    [[nodiscard]] static auto
    ParseSingleArrayDestructureItem(ParseContext &Context) noexcept
        -> AST::ArrayDestructureItem *
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
        // `ParseRightSideOfArrayDestructureItemColon`.

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
                return ParseRightSideOfArrayDestructureItemColon(
                    Context, /*IndexExpr=*/nullptr, IndexLoc);
            }

            TokenStream.goBack();
        }

        if (TokenStream.consumeIfIs(Lex::TokenKind::LeftSquareBracket)) {
            auto ItemLoc = TokenStream.getCurrOrPrevLoc();
            auto ItemListOpt = ParseArrayDestructureItemList(Context);

            if (!ItemListOpt.has_value()) {
                return nullptr;
            }

            auto &ItemList = ItemListOpt.value();
            return new AST::ArrayDestructureItemArray(Qualifiers,
                                                      /*Index=*/std::nullopt,
                                                      std::move(ItemList),
                                                      ItemLoc);
        }

        if (TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace)) {
            auto ItemLoc = TokenStream.getCurrOrPrevLoc();
            auto ItemListOpt = ParseObjectDestructureFieldList(Context);

            if (!ItemListOpt.has_value()) {
                return nullptr;
            }

            auto &ItemList = ItemListOpt.value();
            return new AST::ArrayDestructureItemObject(Qualifiers,
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

                return nullptr;
            }

            const auto NameToken = NameTokenOpt.value();
            if (NameToken.Kind == Lex::TokenKind::RightSquareBracket) {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = TokenStream.getCurrOrPrevLoc(),
                    .Message = "Spread operator missing a name"
                });

                return nullptr;
            }

            if (!VerifyDeclName(Context, NameToken, "variable")) {
                return nullptr;
            }

            const auto Name = TokenStream.tokenContent(NameToken);
            const auto Result =
                new AST::ArrayDestructureItemSpread(Qualifiers,
                                                    /*Index=*/std::nullopt,
                                                    Name, NameToken.Loc,
                                                    ItemLoc);

            return Result;
        }

        const auto IndexLoc = TokenStream.getCurrOrPrevLoc();
        const auto IndexExpr = ParseExpression(Context);

        if (IndexExpr == nullptr) {
            return nullptr;
        }

        if (TokenStream.consumeIfIs(Lex::TokenKind::Colon)) {
            const auto DestructureItem =
                ParseRightSideOfArrayDestructureItemColon(Context, IndexExpr,
                                                          IndexLoc);

            return DestructureItem;
        }

        // FIXME: We're missing a colon here, recover here.
        return nullptr;
    }

    [[nodiscard]]
    static auto ParseArrayDestructureItemList(ParseContext &Context) noexcept
        -> std::expected<std::vector<AST::ArrayDestructureItem *>, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto DestructureItemList = std::vector<AST::ArrayDestructureItem *>();
        do {
            const auto DestructureItem =
                ParseSingleArrayDestructureItem(Context);

            if (DestructureItem != nullptr) {
                DestructureItemList.emplace_back(DestructureItem);
                if (TokenStream.consumeIfIs(Lex::TokenKind::RightSquareBracket))
                {
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
            }

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
        return DestructureItemList;
    }

    [[nodiscard]]
    static auto ParseRightSideForVarDecl(ParseContext &Context) noexcept
        -> std::expected<AST::Expr *, ParseError>
    {
        const auto InitExpr =
            ParseInitExpressionIfFound(Context, Lex::TokenKind::Semicolon);

        if (InitExpr != nullptr) {
            return InitExpr;
        }

        if (ProceedToSemicolon(Context)) {
            return std::unexpected(ParseError::FailedAndProceeded);
        }

        return std::unexpected(ParseError::FailedCouldNotProceed);
    }

    [[nodiscard]] static auto
    ParseArrayDestructureVarDecl(ParseContext &Context,
                                 const AST::Qualifiers &PreQualifiers,
                                 const SourceLocation SquareBracketLoc) noexcept
        -> std::expected<AST::Stmt *, ParseError>
    {
        auto Qualifiers = AST::Qualifiers();
        ParseQualifiers(Context, Qualifiers);

        auto DestructureItemListOpt = ParseArrayDestructureItemList(Context);
        if (!DestructureItemListOpt.has_value()) {
            return std::unexpected(DestructureItemListOpt.error());
        }

        const auto InitExprOpt = ParseRightSideForVarDecl(Context);
        if (!InitExprOpt.has_value()) {
            return std::unexpected(InitExprOpt.error());
        }

        auto &DestructureItemList = DestructureItemListOpt.value();
        return new AST::ArrayDestructuredVarDecl(SquareBracketLoc, Qualifiers,
                                                 std::move(DestructureItemList),
                                                 InitExprOpt.value());
    }

    [[nodiscard]] static
    auto ParseSingleObjectDestructureField(ParseContext &Context) noexcept
        -> AST::ObjectDestructureField *
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

            return nullptr;
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

                return nullptr;
            }

            const auto NameToken = NameTokenOpt.value();
            if (!VerifyDeclName(Context, NameToken, "variable")) {
                return nullptr;
            }

            const auto Name = TokenStream.tokenContent(NameToken);
            const auto Result =
                new AST::ObjectDestructureFieldSpread(Name, NameToken.Loc,
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

            return nullptr;
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

            return new AST::ObjectDestructureFieldIdentifier(
                Key, KeyLoc, Name, NameLoc, AtKeyLocQualifiers);
        }

        // FIXME: Do something with the previous qualifier values.

        AtKeyLocQualifiers.clear();
        ParseQualifiers(Context, AtKeyLocQualifiers);

        const auto Key = TokenStream.tokenContent(KeyToken);
        if (TokenStream.consumeIfIs(Lex::TokenKind::LeftSquareBracket)) {
            auto ArrayDestructureItemListOpt =
                ParseArrayDestructureItemList(Context);

            if (!ArrayDestructureItemListOpt.has_value()) {
                // FIXME: Properly recover here.
                return nullptr;
            }

            auto &ArrayDestructureItemList =
                ArrayDestructureItemListOpt.value();

            return new AST::ObjectDestructureFieldArray(
                Key, KeyLoc, AtKeyLocQualifiers,
                std::move(ArrayDestructureItemList));
        }

        if (TokenStream.consumeIfIs(Lex::TokenKind::OpenCurlyBrace)) {
            auto ObjectDestructureFieldListOpt =
                ParseObjectDestructureFieldList(Context);

            if (!ObjectDestructureFieldListOpt.has_value()) {
                // FIXME: Properly recover here.
                return nullptr;
            }

            auto &ObjectDestructureFieldList =
                ObjectDestructureFieldListOpt.value();

            return new AST::ObjectDestructureFieldObject(
                Key, KeyLoc, AtKeyLocQualifiers,
                std::move(ObjectDestructureFieldList));
        }

        const auto NameTokenOpt = TokenStream.consume();
        if (!NameTokenOpt.has_value()) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected a name for object-like destructuring"
            });

            return nullptr;
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

            return nullptr;
        }

        const auto Name = TokenStream.tokenContent(NameToken);
        return new AST::ObjectDestructureFieldIdentifier(Key, KeyLoc, Name,
                                                         NameToken.Loc,
                                                         AtKeyLocQualifiers);
    }

    static auto
    ParseObjectDestructureFieldList(ParseContext &Context) noexcept
        -> std::expected<std::vector<AST::ObjectDestructureField *>, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto DestructureItemList = std::vector<AST::ObjectDestructureField *>();
        do {
            const auto DestructureItem =
                ParseSingleObjectDestructureField(Context);

            if (DestructureItem != nullptr) {
                DestructureItemList.emplace_back(DestructureItem);
                if (TokenStream.consumeIfIs(Lex::TokenKind::CloseCurlyBrace)) {
                    break;
                }

                if (TokenStream.consumeIfIs(Lex::TokenKind::Comma)) {
                    continue;
                }
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
        return DestructureItemList;
    }

    static auto
    ParseObjectDestructureVarDecl(ParseContext &Context,
                                  const AST::Qualifiers &PreQualifiers,
                                  const SourceLocation CurlyLoc) noexcept
        -> std::expected<AST::Stmt *, ParseError>
    {
        auto Qualifiers = AST::Qualifiers();
        ParseQualifiers(Context, Qualifiers);

        auto DestructureItemListOpt = ParseObjectDestructureFieldList(Context);
        if (!DestructureItemListOpt.has_value()) {
            return std::unexpected(DestructureItemListOpt.error());
        }

        auto &DestructureItemList = DestructureItemListOpt.value();
        const auto InitExprOpt = ParseRightSideForVarDecl(Context);

        if (!InitExprOpt.has_value()) {
            return std::unexpected(InitExprOpt.error());
        }

        const auto InitExpr = InitExprOpt.value();
        return new AST::ObjectDestructuredVarDecl(CurlyLoc, Qualifiers,
                                                  std::move(
                                                    DestructureItemList),
                                                  InitExpr);
    }

    auto
    ParseVarDecl(ParseContext &Context,
                 const AST::Qualifiers &PreQualifiers) noexcept
        -> std::expected<AST::Stmt *, ParseError>
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        auto Qualifiers = AST::Qualifiers();
        auto DeclLoc = SourceLocation();

        ParseQualifiers(Context, Qualifiers);

        const auto NameTokenOpt = TokenStream.consume();
        if (NameTokenOpt.has_value()) {
            const auto NameToken = NameTokenOpt.value();
            if (NameToken.Kind == Lex::TokenKind::LeftSquareBracket) {
                return ParseArrayDestructureVarDecl(Context, PreQualifiers,
                                                    NameToken.Loc);
            }

            if (NameToken.Kind == Lex::TokenKind::OpenCurlyBrace) {
                return ParseObjectDestructureVarDecl(Context, PreQualifiers,
                                                     NameToken.Loc);
            }

            if (!VerifyDeclName(Context, NameToken, "variable")) {
                return nullptr;
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

        const auto TypeExpr =
            ParseTypeAnnotationIfFound(Context, DeclLoc,
                                       Lex::TokenKind::Semicolon);

        if (!TokenStream.consumeIfIs(Lex::TokenKind::Equal)) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Variable declarations must have an initial value"
            });

            return nullptr;
        }

        const auto InitExpr = ParseExpression(Context);
        if (!ExpectSemicolon(Context)) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = TokenStream.getCurrOrPrevLoc(),
                .Message = "Expected ';' after variable declaration"
            });

            return nullptr;
        }

        const auto NameLoc =
            NameTokenOpt.has_value() ? NameTokenOpt.value().Loc : DeclLoc;
        const auto Name =
            NameTokenOpt.has_value() ?
                TokenStream.tokenContent(NameTokenOpt.value()) :
                std::string_view();

        return new AST::VarDecl(Name, NameLoc, Qualifiers, TypeExpr, InitExpr);
    }
}
