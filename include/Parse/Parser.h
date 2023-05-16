/*
 * Parse/Parser.h
 */

#pragma once

#include <optional>
#include <string_view>

#include "AST/CharLiteral.h"
#include "AST/CompoundStmt.h"
#include "AST/Context.h"
#include "AST/FunctionCall.h"
#include "AST/FunctionDecl.h"
#include "AST/FunctionProtoype.h"
#include "AST/IfStmt.h"
#include "AST/NumberLiteral.h"
#include "AST/ParenExpr.h"
#include "AST/ReturnStmt.h"
#include "AST/StringLiteral.h"
#include "AST/UnaryOperation.h"
#include "AST/VarDecl.h"

#include "Basic/SourceManager.h"
#include "Interface/DiagnosticsEngine.h"

namespace Parse {
    struct ParserOptions {
        bool DontRequireSemicolons : 1 = false;
    };

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

        [[nodiscard]]
        auto
        parseFunctionCall(Lex::Token NameToken, Lex::Token ParenToken) noexcept
            -> AST::FunctionCall *;

        [[nodiscard]]
        auto parseIdentifierForLhs(Lex::Token Token) noexcept -> AST::Expr *;

        [[nodiscard]] auto parseLHS() noexcept -> AST::Expr *;

        [[nodiscard]]
        auto parseBinOpRHS(AST::Expr *LHS, uint64_t MinPrec) noexcept
            -> AST::Expr *;

        [[nodiscard]]
        auto parseIfStmt(Lex::Token IfToken) noexcept -> AST::IfStmt *;

        [[nodiscard]]
        auto parseReturnStmt(Lex::Token ReturnToken) noexcept
            -> AST::ReturnStmt *;

        [[nodiscard]]
        auto parseCompoundStmt(Lex::Token CurlyToken) noexcept
            -> AST::CompoundStmt *;

        [[nodiscard]]
        auto parseStmt(const bool ParseTopLevelExpr = false) noexcept
            -> AST::Stmt *;

        [[nodiscard]] auto parseExpression() noexcept -> AST::Expr *;
        [[nodiscard]] auto parseExpressionAndEnd() noexcept -> AST::Expr *;
        [[nodiscard]] auto parseFuncPrototype() noexcept
            -> AST::FunctionPrototype *;

        [[nodiscard]] auto parseVarDecl() noexcept -> AST::VarDecl *;
        [[nodiscard]] auto parseFuncDecl() noexcept -> AST::FunctionDecl *;

        [[nodiscard]] auto peek() -> std::optional<Lex::Token>;
        [[nodiscard]] auto prev() -> std::optional<Lex::Token>;

        [[nodiscard]]
        auto consumeIf(Lex::TokenKind Kind) -> std::optional<Lex::Token>;

        [[nodiscard]]
        auto tokenContent(Lex::Token Token) const noexcept -> std::string_view;

        auto consume(uint64_t Skip = 0) -> std::optional<Lex::Token>;
        auto expect(Lex::TokenKind Kind, bool Optional = false) -> bool;

        const SourceManager &SourceMngr;
        AST::Context &Context;

        const std::vector<Lex::Token> &TokenList;
        Interface::DiagnosticsEngine &Diag;

        uint64_t Index = 0;
        ParserOptions Options;
    public:
        constexpr explicit
        Parser(const SourceManager &SourceMngr,
               AST::Context &Context,
               const std::vector<Lex::Token> &TokenList,
               Interface::DiagnosticsEngine &Diag,
               ParserOptions Options = {}) noexcept
        : SourceMngr(SourceMngr), Context(Context), TokenList(TokenList),
          Diag(Diag), Options(Options) {}

        auto startParsing() noexcept -> bool;
        auto parseTopLevelExpressionOrStmt() noexcept -> AST::Expr *;

        [[nodiscard]] constexpr const auto &tokenList() const noexcept {
            return TokenList;
        }

        [[nodiscard]] constexpr auto position() const noexcept {
            return Index;
        }
    };
}