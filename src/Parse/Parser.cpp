/*
 * Parse/Parser.cpp
 */

#include "AST/BinaryOperation.h"
#include "AST/VariableRef.h"

#include "Lex/Token.h"

#include "Parse/OperatorPrecedence.h"
#include "Parse/Parser.h"
#include "Parse/String.h"

namespace Parse {
    auto Parser::peek() -> std::optional<Lex::Token> {
        if (position() >= tokenList().size()) {
            return std::nullopt;
        }

        return tokenList().at(position());
    }

    [[nodiscard]] auto Parser::prev() -> std::optional<Lex::Token> {
        if (position() - 1 >= tokenList().size()) {
            return std::nullopt;
        }

        return tokenList().at(position() - 1);
    }

    auto Parser::consume(const uint64_t Skip) -> std::optional<Lex::Token> {
        if (position() + Skip >= tokenList().size()) {
            return std::nullopt;
        }

        this->Index += Skip + 1;
        return tokenList().at(position() - 1);
    }

    [[nodiscard]]
    auto Parser::consumeIf(const Lex::TokenKind Kind)
        -> std::optional<Lex::Token>
    {
        if (const auto TokenOpt = this->peek()) {
            const auto Token = TokenOpt.value();
            if (Token.Kind == Kind) {
                Index++;
                return Token;
            }
        }

        return std::nullopt;
    }

    auto
    Parser::expect(const Lex::TokenKind Kind, const bool Optional) -> bool {
        auto TokenOpt = this->consume();
        if (TokenOpt.has_value()) {
            if (TokenOpt.value().Kind == Kind) {
                if (Kind == Lex::TokenKind::Semicolon) {
                    while ((TokenOpt = this->peek())) {
                        /* Skip past multiple semicolons */
                        const auto Token = TokenOpt.value();
                        if (Token.Kind != Lex::TokenKind::Semicolon) {
                            break;
                        }

                        this->consume();
                    }
                }

                return true;
            }

            if (Kind == Lex::TokenKind::EOFToken) {
                return false;
            }

            this->Index--;
        } else if (Kind == Lex::TokenKind::EOFToken) {
            return true;
        }

        return Optional;
    }

    [[nodiscard]]
    auto Parser::tokenContent(const Lex::Token Token) const noexcept
        -> std::string_view
    {
        return Token.getString(SourceMngr.getText());
    }

    auto Parser::parseUnaryOper(const Lex::Token Token) noexcept
        -> AST::UnaryOperation *
    {
        const auto TokenString = tokenContent(Token);
        const auto UnaryOpOpt =
            Parse::UnaryOperatorToLexemeMap.keyFor(TokenString);

        if (!UnaryOpOpt.has_value()) {
            Diag.emitError("Internal error: unary operator not lexed "
                           "correctly");
            return nullptr;
        }

        return new AST::UnaryOperation(Token.Loc, UnaryOpOpt.value());
    }

    auto Parser::parseNumberLiteral(const Lex::Token Token) noexcept
        -> AST::NumberLiteral *
    {
        const auto ParseResult = Parse::ParseNumber(tokenContent(Token));
        if (ParseResult.Error != ParseNumberError::None) {
            Diag.emitError("Failed to parse number-literal");
            return nullptr;
        }

        return new AST::NumberLiteral(Token.Loc, ParseResult);
    }

    auto Parser::parseCharLiteral(const Lex::Token Token) noexcept
        -> AST::CharLiteral *
    {
        const auto TokenString = tokenContent(Token);
        if (TokenString.size() == 2) {
            Diag.emitError("Characters cannot be empty. Use \'\'0\' to store a "
                           "null-character");
            return nullptr;
        }

        const auto FirstChar = TokenString.at(1);
        if (FirstChar != '\\') {
            if (TokenString.size() != 3) {
                Diag.emitError("Use double quotes to store strings with "
                               "multiple characters");
                return nullptr;
            }

            return new AST::CharLiteral(Token.Loc, FirstChar);
        }

        if (FirstChar == '\\' && TokenString.size() == 3) {
            Diag.emitError(
                "Expected another character to parse escape-sequence. "
                "Use \'\\\\\' to store a back-slash");
            return nullptr;
        }

        const auto EscapedChar = VerifyEscapeSequence(TokenString.at(2));
        if (EscapedChar == '\0') {
            Diag.emitError("Invalid escape-sequence");
            return nullptr;
        }

        return new AST::CharLiteral(Token.Loc, EscapedChar);
    }

    auto Parser::parseStringLiteral(const Lex::Token Token) noexcept
        -> AST::StringLiteral *
    {
        const auto TokenString = tokenContent(Token);
        const auto TokenStringLiteral =
            TokenString.substr(1, TokenString.size() - 1);

        auto String = std::string();
        String.reserve(TokenStringLiteral.size());

        for (auto I = uint64_t(); I != TokenStringLiteral.size(); ++I) {
            auto Char = TokenStringLiteral.at(I);
            if (Char == '\\') {
                if (I + 1 == TokenStringLiteral.size()) {
                    Diag.emitError("Expected another character to parse "
                                   "escape-sequence");
                    return nullptr;
                }

                I++;
                const auto NextChar =
                    VerifyEscapeSequence(TokenStringLiteral.at(I));

                if (NextChar == '\0') {
                    Diag.emitError("Invalid escape-sequence");
                    return nullptr;
                }

                Char = NextChar;
            }

            String.append(1, Char);
        }

        return new AST::StringLiteral(Token.Loc, String);
    }

    auto Parser::parseParenExpr(const Lex::Token Token) noexcept
        -> AST::ParenExpr *
    {
        const auto ChildExpr = this->parseExpression();
        if (!this->expect(Lex::TokenKind::CloseParen)) {
            Diag.emitError("Expected closing parenthesis");
            return nullptr;
        }

        return new AST::ParenExpr(Token, ChildExpr);
    }

    auto
    Parser::parseFunctionCall(const Lex::Token NameToken,
                              const Lex::Token ParenToken) noexcept
        -> AST::FunctionCall *
    {
        auto ArgList = std::vector<AST::Expr *>();
        do {
            const auto TokenOpt = this->peek();
            if (!TokenOpt.has_value()) {
                Diag.emitError("Unexpected end of file while parsing function "
                               "parameter list");
                return nullptr;
            }

            const auto Token = TokenOpt.value();
            if (Token.Kind == Lex::TokenKind::CloseParen) {
                this->consume();
                break;
            }

            if (const auto Expr = this->parseExpression()) {
                ArgList.emplace_back(Expr);
            } else {
                return nullptr;
            }

            const auto ArgEndTokenOpt = this->consume();
            if (!ArgEndTokenOpt.has_value()) {
                Diag.emitError("Expected end of function parameter list");
                return nullptr;
            }

            const auto ArgEndToken = ArgEndTokenOpt.value();
            if (ArgEndToken.Kind == Lex::TokenKind::CloseParen) {
                break;
            }

            if (ArgEndToken.Kind != Lex::TokenKind::Comma) {
                Diag.emitError("Expected comma or closing parenthesis");
                return nullptr;
            }
        } while (true);

        return new AST::FunctionCall(NameToken.Loc,
                                     tokenContent(NameToken),
                                     std::move(ArgList));
    }

    auto
    Parser::parseIdentifierForLhs(const Lex::Token IdentToken) noexcept
        -> AST::Expr *
    {
        if (const auto TokenOpt = this->consumeIf(Lex::TokenKind::OpenParen)) {
            return this->parseFunctionCall(IdentToken, TokenOpt.value());
        }

        return new AST::VariableRef(IdentToken.Loc, tokenContent(IdentToken));
    }

    auto Parser::parseLHS() noexcept -> AST::Expr * {
        /* Parse Prefixes and then LHS */
        auto Root = static_cast<AST::Expr *>(nullptr);
        auto CurrentOper = static_cast<AST::UnaryOperation *>(nullptr);

        while (true) {
            const auto TokenOpt = this->consume();
            if (!TokenOpt.has_value()) {
                return Root;
            }

            const auto Token = TokenOpt.value();
            auto Expr = static_cast<AST::Expr *>(nullptr);

            if (!Lex::TokenKindIsUnaryOp(Token.Kind) &&
                Token.Kind != Lex::TokenKind::Minus)
            {
                switch (Token.Kind) {
                    case Lex::TokenKind::Identifier:
                        Expr = this->parseIdentifierForLhs(Token);
                        break;
                    case Lex::TokenKind::Keyword:
                        Diag.emitError("Keyword \"" SV_FMT "\" cannot be used "
                                       "in an expression",
                                       SV_FMT_ARG(tokenContent(Token)));
                        return nullptr;
                    case Lex::TokenKind::NumberLiteral:
                        Expr = this->parseNumberLiteral(Token);
                        break;
                    case Lex::TokenKind::CharLiteral:
                        Expr = this->parseCharLiteral(Token);
                        break;
                    case Lex::TokenKind::StringLiteral:
                        Expr = this->parseStringLiteral(Token);
                        break;
                    case Lex::TokenKind::OpenParen:
                        Expr = this->parseParenExpr(Token);
                        break;
                    default:
                        Diag.emitError("Unexpected token \"" SV_FMT "\"",
                                       SV_FMT_ARG(tokenContent(Token)));
                        return nullptr;
                }
            } else {
                Expr = this->parseUnaryOper(Token);
            }

            if (Expr == nullptr) {
                return nullptr;
            }

            if (Root == nullptr) {
                Root = Expr;
            } else {
                CurrentOper->setOperand(Expr);
            }

            if (Expr->getKind() == AST::NodeKind::UnaryOperation) {
                CurrentOper = static_cast<AST::UnaryOperation *>(Expr);
                continue;
            }

            return Root;
        }

        assert(false && "unreachable");
    }

    auto Parser::parseBinOpRHS(AST::Expr *LHS, const uint64_t MinPrec) noexcept
        -> AST::Expr *
    {
        do {
            auto TokenOpt = this->peek();
            if (!TokenOpt.has_value()) {
                return LHS;
            }

            auto Token = TokenOpt.value();
            if (!Lex::TokenKindIsBinOp(Token.Kind)) {
                return LHS;
            }

            const auto ThisOperInfoOpt = Parse::OperatorInfoMap[Token.Kind];
            assert(ThisOperInfoOpt.has_value() &&
                   "OperatorInfoMap is missing a bin-op");

            const auto ThisOperInfo = ThisOperInfoOpt.value();
            if (static_cast<uint64_t>(ThisOperInfo.Precedence) < MinPrec) {
                return LHS;
            }

            this->consume();
            if (!this->peek().has_value()) {
                Diag.emitError("Expected an expression after \"" SV_FMT "\"",
                               SV_FMT_ARG(tokenContent(Token)));
                return nullptr;
            }

            auto RHS = this->parseLHS();
            if (RHS == nullptr) {
                return nullptr;
            }

            const auto BinOpToken = Token;
            if ((TokenOpt = this->peek()) &&
                Lex::TokenKindIsBinOp(TokenOpt.value().Kind))
            {
                Token = TokenOpt.value();

                const auto ThisIsRightAssoc =
                    ThisOperInfo.Assoc == Parse::OperatorAssoc::Right;
                const auto &NextOperInfoOpt =
                    Parse::OperatorInfoMap[Token.Kind];

                assert(NextOperInfoOpt.has_value() &&
                       "Internal Error: OperatorInfoMap missing entry");

                const auto NextOperInfo = NextOperInfoOpt.value();
                if (ThisOperInfo.Precedence < NextOperInfo.Precedence ||
                    (ThisOperInfo.Precedence == NextOperInfo.Precedence &&
                     ThisIsRightAssoc))
                {
                    const auto NextMinPrec =
                        static_cast<uint64_t>(
                            static_cast<uint64_t>(ThisOperInfo.Precedence) +
                            !ThisIsRightAssoc);

                    RHS = this->parseBinOpRHS(RHS, NextMinPrec);
                    if (RHS == nullptr) {
                        return RHS;
                    }
                }
            }

            const auto BinOp = LexTokenKindToBinaryOperatorMap[BinOpToken.Kind];
            assert(BinOp.has_value() &&
                   "Internal Error: LexTokenKindToBinaryOperatorMap missing "
                   "BinOp");

            LHS = new AST::BinaryOperation(Token.Loc, BinOp.value(), LHS, RHS);
        } while (true);
    }

    auto Parser::parseExpression() noexcept -> AST::Expr * {
        if (!this->peek().has_value()) {
            Diag.emitError("Expected an expression");
            return nullptr;
        }

        return this->parseBinOpRHS(this->parseLHS(), /*MinPrec=*/0);
    }

    auto Parser::parseExpressionAndEnd() noexcept -> AST::Expr * {
        if (const auto Expr = this->parseExpression()) {
            if (!this->expect(Lex::TokenKind::Semicolon,
                              /*Optional=*/Options.DontRequireSemicolons))
            {
                Diag.emitError("Expected a semicolon after expression");
                return nullptr;
            }

            return Expr;
        }

        return nullptr;
    }

    auto Parser::parseVarDecl() noexcept -> AST::VarDecl * {
        const auto NameTokenOpt = this->consume();
        if (!NameTokenOpt.has_value()) {
            Diag.emitError("Expected a name for variable declaration");
            return nullptr;
        }

        const auto NameToken = NameTokenOpt.value();
        switch (NameToken.Kind) {
            case Lex::TokenKind::Identifier:
                break;
            case Lex::TokenKind::Keyword:
                Diag.emitError("Keyword \"" SV_FMT "\" cannot be used as a "
                               "variable name",
                               SV_FMT_ARG(tokenContent(NameToken)));
                return nullptr;
            case Lex::TokenKind::NumberLiteral:
                Diag.emitError("Number Literal \"" SV_FMT "\" cannot be used "
                               "as a variable name",
                               SV_FMT_ARG(tokenContent(NameToken)));
                return nullptr;
            default:
                Diag.emitError("Expected a variable name, got \"" SV_FMT "\" "
                               "instead",
                               SV_FMT_ARG(tokenContent(NameToken)));
                return nullptr;
        }

        if (!this->expect(Lex::TokenKind::Equal)) {
            Diag.emitError("Variable declaration must have an initial value");
            return nullptr;
        }

        const auto Expr = this->parseExpressionAndEnd();
        if (Expr == nullptr) {
            return nullptr;
        }

        return new AST::VarDecl(NameToken, tokenContent(NameToken), Expr);
    }

    auto Parser::parseFuncPrototype() noexcept -> AST::FunctionPrototype * {
        const auto NameTokenOpt = this->consume();
        if (!NameTokenOpt.has_value()) {
            Diag.emitError("Expected a name for function declaration");
            return nullptr;
        }

        const auto NameToken = NameTokenOpt.value();
        switch (NameToken.Kind) {
            case Lex::TokenKind::Identifier:
                break;
            case Lex::TokenKind::Keyword:
                Diag.emitError("Keyword \"" SV_FMT "\" cannot be used as a "
                               "function name",
                               SV_FMT_ARG(tokenContent(NameToken)));
                return nullptr;
            case Lex::TokenKind::NumberLiteral:
                Diag.emitError("Number Literal \"" SV_FMT "\" cannot be used "
                               "as a function name",
                               SV_FMT_ARG(tokenContent(NameToken)));
                return nullptr;
            default:
                Diag.emitError("Expected a function name, got \"" SV_FMT "\""
                               "instead",
                               SV_FMT_ARG(tokenContent(NameToken)));
                return nullptr;
        }

        if (!this->expect(Lex::TokenKind::OpenParen)) {
            Diag.emitError("Expected an parenthesis after function name");
            return nullptr;
        }

        auto ParamList = std::vector<AST::FunctionPrototype::ParamDecl>();
        if (const auto CloseParenOpt =
                this->consumeIf(Lex::TokenKind::CloseParen))
        {
            return new AST::FunctionPrototype(NameToken.Loc,
                                                tokenContent(NameToken),
                                                ParamList);
        }

        do {
            const auto TokenOpt = this->consume();
            if (!TokenOpt.has_value()) {
                Diag.emitError("Unexpected end of file while parsing function "
                               "parameter list");
                return nullptr;
            }

            const auto Token = TokenOpt.value();
            if (Token.Kind != Lex::TokenKind::Identifier) {
                Diag.emitError("Unexpected token \"" SV_FMT "\" while parsing ",
                               SV_FMT_ARG(tokenContent(Token)));
                return nullptr;
            }

            ParamList.emplace_back(Token.Loc, tokenContent(Token));

            const auto CommaOrCloseParenOpt = this->consume();
            if (!CommaOrCloseParenOpt.has_value()) {
                Diag.emitError("Expected a comma or closing parenthesis");
                return nullptr;
            }

            const auto CommaOrCloseParen = CommaOrCloseParenOpt.value();
            if (CommaOrCloseParen.Kind == Lex::TokenKind::CloseParen) {
                break;
            }

            if (CommaOrCloseParen.Kind != Lex::TokenKind::Comma) {
                Diag.emitError("Expected a comma or closing parenthesis");
                return nullptr;
            }
        } while (true);

        return new AST::FunctionPrototype(NameToken.Loc,
                                          tokenContent(NameToken),
                                          ParamList);
    }

    auto Parser::parseFuncDecl() noexcept -> AST::FunctionDecl * {
        if (const auto Proto = this->parseFuncPrototype()) {
            if (const auto Body = this->parseStmt()) {
                return new AST::FunctionDecl(Proto, Body);
            }
        }

        return nullptr;
    }

    auto Parser::parseIfStmt(const Lex::Token IfToken) noexcept -> AST::IfStmt *
    {
        if (!this->expect(Lex::TokenKind::OpenParen)) {
            Diag.emitError("Expected an opening parenthesis after if");
            return nullptr;
        }

        const auto Condition = this->parseExpression();
        if (Condition == nullptr) {
            return nullptr;
        }

        if (!this->expect(Lex::TokenKind::CloseParen)) {
            Diag.emitError("Expected a closing parenthesis after if condition");
            return nullptr;
        }

        const auto Then = this->parseStmt();
        if (Then == nullptr) {
            return nullptr;
        }

        auto ElseStmt = static_cast<AST::Stmt *>(nullptr);
        if (const auto ElseTokenOpt =
                this->consumeIf(Lex::TokenKind::Keyword))
        {
            const auto TokenString = tokenContent(ElseTokenOpt.value());
            if (Lex::TokenStringIsKeyword(TokenString, Lex::Keyword::Else)) {
                ElseStmt = this->parseStmt();
                if (ElseStmt == nullptr) {
                    return nullptr;
                }
            }
        }

        return new AST::IfStmt(IfToken.Loc, Condition, Then, ElseStmt);
    }

    auto
    Parser::parseReturnStmt(const Lex::Token ReturnToken) noexcept
        -> AST::ReturnStmt *
    {
        if (const auto Expr = this->parseExpressionAndEnd()) {
            return new AST::ReturnStmt(ReturnToken.Loc, Expr);
        }

        return nullptr;
    }

    auto Parser::parseCompoundStmt(const Lex::Token CurlyToken) noexcept
        -> AST::CompoundStmt *
    {
        auto StmtList = std::vector<AST::Stmt *>();
        while (true) {
            const auto TokenOpt = this->peek();
            if (!TokenOpt.has_value()) {
                Diag.emitError("Unexpected end of file");
                return nullptr;
            }

            if (this->consumeIf(Lex::TokenKind::CloseCurlyBrace)) {
                break;
            }

            if (const auto Stmt = this->parseStmt()) {
                StmtList.emplace_back(Stmt);
                continue;
            }

            return nullptr;
        }

        return new AST::CompoundStmt(CurlyToken.Loc, std::move(StmtList));
    }

    auto Parser::parseStmt() noexcept -> AST::Stmt * {
        const auto TokenOpt = this->consume();
        if (!TokenOpt.has_value()) {
            Diag.emitError("Unexpected end of file");
            return nullptr;
        }

        const auto Token = TokenOpt.value();
        switch (Token.Kind) {
            case Lex::TokenKind::Keyword: {
                const auto TokenString = tokenContent(Token);
                switch (Lex::KeywordTokenGetKeyword(TokenString)) {
                    case Lex::Keyword::Let:
                        return this->parseVarDecl();
                    case Lex::Keyword::Function:
                        return this->parseFuncDecl();
                    case Lex::Keyword::If:
                        return this->parseIfStmt(Token);
                    case Lex::Keyword::Else:
                        return this->parseExpressionAndEnd();
                    case Lex::Keyword::Return:
                        return this->parseReturnStmt(Token);
                }

                [[fallthrough]];
            }
            case Lex::TokenKind::OpenCurlyBrace:
                return this->parseCompoundStmt(Token);
            default:
                Index--;
                return this->parseExpressionAndEnd();
        }

        return nullptr;
    }

    auto Parser::startParsing() noexcept -> bool {
        while (const auto Token = this->peek()) {
            if (const auto Stmt = this->parseStmt()) {
                if (const auto DeclStmt = llvm::dyn_cast<AST::Decl>(Stmt)) {
                    this->Context.addDecl(DeclStmt);
                    continue;
                }

                Diag.emitError("Unexpected non-declaration statement");
                return false;
            }

            Diag.emitError("Unexpected non-statement");
            return false;
        }

        return true;
    }

    auto Parser::parseTopLevelExpressionOrStmt() noexcept -> AST::Stmt * {
        if (const auto Result = this->parseStmt()) {
            if (const auto Token = this->peek()) {
                Diag.emitError("Unexpected token \"" SV_FMT "\"",
                               SV_FMT_ARG(tokenContent(*Token)));
                return nullptr;
            }

            if (const auto Decl = llvm::dyn_cast<AST::Decl>(Result)) {
                Context.addDecl(Decl);
            }

            return Result;
        }

        return nullptr;
    }
}