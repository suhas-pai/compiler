/*
 * Parse/Parser.h
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "AST/CharLiteral.h"
#include "AST/NumberLiteral.h"
#include "AST/ParenExpr.h"
#include "AST/StringLiteral.h"
#include "AST/UnaryOperation.h"
#include "AST/VarDecl.h"
#include "Lex/Token.h"

namespace Parse {
    struct Parser {
    protected:
        [[nodiscard]] auto
        parseUnaryOper(Lex::Token Token) noexcept -> AST::UnaryOperation *;

        [[nodiscard]] auto
        parseNumberLiteral(Lex::Token Token) noexcept -> AST::NumberLiteral *;

        [[nodiscard]]
        auto parseCharLiteral(Lex::Token Token) noexcept -> AST::CharLiteral *;

        [[nodiscard]] auto
        parseStringLiteral(Lex::Token Token) noexcept -> AST::StringLiteral *;

        [[nodiscard]]
        auto parseParenExpr(Lex::Token Token) noexcept -> AST::ParenExpr *;

        [[nodiscard]] auto parseLHS() noexcept -> AST::Expr *;
        [[nodiscard]]
        auto parseBinOpRHS(AST::Expr *LHS, uint64_t MinPrec) noexcept
            -> AST::Expr *;

        [[nodiscard]] auto parseStmt() noexcept -> AST::Stmt *;
        [[nodiscard]] auto parseExpression() noexcept -> AST::Expr *;
        [[nodiscard]] auto parseVarDecl() noexcept -> AST::VarDecl *;

        [[nodiscard]] auto peek() -> std::optional<Lex::Token>;
        [[nodiscard]] auto prev() -> std::optional<Lex::Token>;

        auto consume(uint64_t Skip = 0) -> std::optional<Lex::Token>;
        auto expect(Lex::TokenKind Kind) -> bool;

        const std::string &Text;
        const std::vector<Lex::Token> &TokenList;

        uint64_t Index = 0;
    public:
        constexpr explicit
        Parser(const std::string &Text,
               const std::vector<Lex::Token> &TokenList) noexcept
        : Text(Text), TokenList(TokenList) {}

        auto startParsing() noexcept -> AST::Expr *;

        [[nodiscard]] constexpr const auto &tokenList() const noexcept {
            return TokenList;
        }

        [[nodiscard]] constexpr auto position() const noexcept {
            return Index;
        }
    };
}