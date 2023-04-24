/*
 * Parse/Parser.cpp
 */

#include "AST/BinaryOperation.h"
#include "Lex/Keyword.h"

#include "Parse/OperatorPrecedence.h"
#include "Parse/String.h"
#include "Parse/Parser.h"

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

    auto Parser::expect(const Lex::TokenKind Kind) -> bool {
        const auto TokenOpt = this->consume();
        return (TokenOpt.has_value() && TokenOpt.value().Kind == Kind);
    }

    auto Parser::parseUnaryOper(const Lex::Token Token) noexcept
        -> AST::UnaryOperation *
    {
        const auto TokenString = Token.getString(Text);
        const auto UnaryOpOpt =
            Parse::UnaryOperatorToLexemeMap.keyFor(TokenString);

        if (!UnaryOpOpt.has_value()) {
            printf("internal error: unary operator not lexed correctly\n");
            exit(1);
        }

        return new AST::UnaryOperation(Token.Loc, UnaryOpOpt.value());
    }

    auto Parser::parseIntegerLiteral(const Lex::Token Token) noexcept
        -> AST::IntegerLiteral *
    {
        const auto ParseResult = Parse::ParseNumber(Token.getString(Text));
        if (ParseResult.Error != ParseNumberError::None) {
            printf("Failed to parse integer-literal\n");
            exit(1);
        }

        return new AST::IntegerLiteral(Token.Loc, ParseResult);
    }

    auto Parser::parseCharLiteral(const Lex::Token Token) noexcept
        -> AST::CharLiteral *
    {
        const auto TokenString = Token.getString(Text);
        if (TokenString.size() == 2) {
            printf("Characters cannot be empty. Use \'\'0\' to store a "
                   "null-character\n");
            exit(1);
        }

        const auto FirstChar = TokenString.at(1);
        if (FirstChar != '\\') {
            if (TokenString.size() != 3) {
                printf("Use double quotes to store strings with multiple "
                    "characters\n");
                exit(1);
            }

            return new AST::CharLiteral(Token.Loc, FirstChar);
        }

        if (FirstChar == '\\' && TokenString.size() == 3) {
            printf("Expected another character to parse escape-sequence. "
                   "Use \'\\\\\' to store a back-slash\n");
            exit(1);
        }

        const auto EscapedChar = VerifyEscapeSequence(TokenString.at(2));
        if (EscapedChar == '\0') {
            printf("Invalid escape-sequence\n");
            exit(1);
        }

        return new AST::CharLiteral(Token.Loc, EscapedChar);
    }

    auto Parser::parseStringLiteral(const Lex::Token Token) noexcept
        -> AST::StringLiteral *
    {
        const auto TokenString = Token.getString(Text);
        const auto TokenStringLiteral =
            TokenString.substr(1, TokenString.size() - 1);

        auto String = std::string();
        String.reserve(TokenStringLiteral.size());

        for (auto I = uint64_t(); I != TokenStringLiteral.size() - 1; ++I) {
            auto Char = TokenStringLiteral.at(I);
            if (TokenStringLiteral.at(I) == '\\') {
                if (I + 1 == TokenStringLiteral.size()) {
                    printf("Expected another character to parse "
                           "escape-sequence.\n");
                    exit(1);
                }

                I++;
                const auto NextChar =
                    VerifyEscapeSequence(TokenStringLiteral.at(I));

                if (NextChar == '\0') {
                    printf("Invalid escape-sequence\n");
                    exit(1);
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
            printf("Expected closing parenthesis\n");
            exit(1);
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
                        printf("Variable reference not yet supported");
                        exit(1);
                    case Lex::TokenKind::Keyword:
                        printf("Keyword \"%s\" cannot be used in an "
                               "expression\n",
                               Token.getString(Text).data());
                        exit(1);
                    case Lex::TokenKind::IntegerLiteral:
                        Expr = this->parseIntegerLiteral(Token);
                        break;
                    case Lex::TokenKind::FloatLiteral:
                        printf("Floating point literals not yet supported");
                        exit(1);
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
                        printf("Unexpected token \"%s\"\n",
                               Token.getString(Text).data());
                        exit(1);
                }
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
            LHS = new AST::BinaryOperation(Token.Loc, BinOp.value(), LHS, RHS);
        } while (true);
    }

    auto Parser::parseExpression() noexcept -> AST::Expr * {
        return this->parseBinOpRHS(this->parseLHS(), /*MinPrec=*/0);
    }

    auto Parser::parseVarDecl() noexcept -> AST::VarDecl * {
        const auto NameTokenOpt = this->consume();
        if (!NameTokenOpt.has_value()) {
            printf("Expected a name for variable declaration\n");
            exit(1);
        }

        const auto NameToken = NameTokenOpt.value();
        switch (NameToken.Kind) {
            case Lex::TokenKind::Identifier:
                break;
            case Lex::TokenKind::Keyword:
                printf("Keyword \"%s\" cannot be used as a variable name\n",
                       NameToken.getString(Text).data());
                exit(1);
            case Lex::TokenKind::IntegerLiteral:
                printf("Integer Literal \"%s\" cannot be used as a variable "
                       "name\n",
                       NameToken.getString(Text).data());
                exit(1);
            default:
                printf("Expected a variable name, got \"%s\" instead\n",
                       NameToken.getString(Text).data());
                exit(1);
        }

        if (!this->expect(Lex::TokenKind::Equal)) {
            printf("Expected an equal sign for variable declaration, got "
                   "\"%s\" instead\n",
                   this->prev().value().getString(Text).data());
            exit(1);
        }

        const auto Name = NameToken.getString(Text);
        const auto Expr = this->parseExpression();

        if (!this->expect(Lex::TokenKind::Semicolon)) {
            printf("Expected a semicolon after variable declaration\n");
            exit(1);
        }

        return new AST::VarDecl(NameToken, Name, Expr);
    }

    auto Parser::parseStmt() noexcept -> AST::Stmt * {
        const auto TokenOpt = this->consume();
        if (!TokenOpt.has_value()) {
            printf("Unexpected end of file\n");
            exit(1);
        }

        const auto Token = TokenOpt.value();
        switch (Token.Kind) {
            case Lex::TokenKind::Keyword: {
                const auto LetKeyword =
                    Lex::KeywordToLexemeMap[Lex::Keyword::Let];

                if (Token.getString(Text) == LetKeyword) {
                    return this->parseVarDecl();
                }

                [[fallthrough]];
            }
            default:
                printf("Unexpected token \"%s\"\n",
                       Token.getString(Text).data());
                exit(1);
        }

        return nullptr;
    }

    auto Parser::startParsing() noexcept -> AST::Expr * {
        return this->parseStmt();
    }
}