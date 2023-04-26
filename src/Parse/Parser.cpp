/*
 * Parse/Parser.cpp
 */

#include "AST/BinaryOperation.h"
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
        if (!this->expect(Lex::TokenKind::RightParen)) {
            Diag.emitError("Expected closing parenthesis");
            return nullptr;
        }

        return new AST::ParenExpr(Token, ChildExpr);
    }

    auto Parser::parseLHS() noexcept -> AST::Expr * {
        /* Parse Prefixes and then LHS */
        auto Root = static_cast<AST::UnaryOperation *>(nullptr);
        auto CurrentOper = static_cast<AST::UnaryOperation *>(nullptr);

        while (true) {
            const auto TokenOpt = this->consume();
            if (!TokenOpt.has_value()) {
                return Root;
            }

            const auto Token = TokenOpt.value();
            auto Expr = static_cast<AST::Expr *>(nullptr);

            if (Lex::TokenKindIsUnaryOp(Token.Kind) ||
                Token.Kind == Lex::TokenKind::Minus)
            {
                Expr = this->parseUnaryOper(Token);
            } else {
                switch (Token.Kind) {
                    case Lex::TokenKind::Identifier:
                        Diag.emitError("Variable reference not yet supported");
                        return nullptr;
                    case Lex::TokenKind::Keyword:
                        Diag.emitError("Keyword \"%s\" cannot be used in an "
                                       "expression",
                                       tokenContent(Token).data());
                        return nullptr;
                    case Lex::TokenKind::NumberLiteral:
                        Expr = this->parseNumberLiteral(Token);
                        break;
                    case Lex::TokenKind::FloatLiteral:
                        Diag.emitError("Floating point literals not yet "
                                       "supported");
                        return nullptr;
                    case Lex::TokenKind::CharLiteral:
                        Expr = this->parseCharLiteral(Token);
                        break;
                    case Lex::TokenKind::StringLiteral:
                        Expr = this->parseStringLiteral(Token);
                        break;
                    case Lex::TokenKind::LeftParen:
                        Expr = this->parseParenExpr(Token);
                        break;
                    default:
                        Diag.emitError("Unexpected token \"%s\"",
                                       tokenContent(Token).data());
                        return nullptr;
                }
            }

            if (Expr == nullptr) {
                return nullptr;
            }

            if (Root == nullptr) {
                Root = static_cast<AST::UnaryOperation *>(Expr);
            } else {
                CurrentOper->setOperand(Expr);
            }

            if (Expr->getKind() == AST::ExprKind::UnaryOperation) {
                CurrentOper = static_cast<AST::UnaryOperation *>(Expr);
                continue;
            }

            return Root;
        }

        assert(false && "unreachable");
    }

    auto
    Parser::parseBinOpRHS(AST::Expr *LHS, const uint64_t MinPrec) noexcept
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
        const auto Lhs = this->parseLHS();
        if (Lhs == nullptr) {
            Diag.emitError("Expected an expression");
            return nullptr;
        }

        return this->parseBinOpRHS(Lhs, /*MinPrec=*/0);
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
                Diag.emitError("Keyword \"%s\" cannot be used as a variable "
                               "name",
                               tokenContent(NameToken).data());
                return nullptr;
            case Lex::TokenKind::NumberLiteral:
                Diag.emitError("Integer Literal \"%s\" cannot be used as a "
                               "variable name",
                               tokenContent(NameToken).data());
                return nullptr;
            default:
                Diag.emitError("Expected a variable name, got \"%s\" instead",
                               tokenContent(NameToken).data());
                return nullptr;
        }

        if (!this->expect(Lex::TokenKind::Equal)) {
            Diag.emitError("Expected an equal sign after name for variable "
                           "declaration");
            return nullptr;
        }

        const auto Name = tokenContent(NameToken);
        const auto Expr = this->parseExpression();

        if (Expr == nullptr) {
            return nullptr;
        }

        if (!this->expect(Lex::TokenKind::Semicolon,
                          /*Optional=*/Options.DontRequireSemicolons))
        {
            Diag.emitError("Expected a semicolon after variable declaration");
            return nullptr;
        }

        if (!this->expect(Lex::TokenKind::EOFToken)) {
            Diag.emitError("Unexpected token past end of statement");
            return nullptr;
        }

        return new AST::VarDecl(NameToken, Name, Expr);
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
                switch (Lex::KeywordTokenGetKeyword(Token, TokenString)) {
                    case Lex::Keyword::Let:
                        return this->parseVarDecl();
                }

                [[fallthrough]];
            }
            default:
                Diag.emitError("Unexpected token \"%s\"",
                               tokenContent(Token).data());
                return nullptr;
        }

        return nullptr;
    }

    auto Parser::startParsing() noexcept -> AST::Expr * {
        return this->parseStmt();
    }
}