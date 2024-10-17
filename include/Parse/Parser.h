/*
 * Parse/Parser.h
 */

#pragma once

#include "AST/Decls/ArrayDecl.h"
#include "AST/Decls/EnumMemberDecl.h"
#include "AST/Decls/FieldDecl.h"
#include "AST/Decls/LvalueNamedDecl.h"
#include "AST/Decls/VarDecl.h"

#include "AST/ArraySubscriptExpr.h"
#include "AST/CallExpr.h"
#include "AST/CharLiteral.h"
#include "AST/CompoundStmt.h"
#include "AST/Context.h"
#include "AST/FieldExpr.h"
#include "AST/IfStmt.h"
#include "AST/NumberLiteral.h"
#include "AST/ParenExpr.h"
#include "AST/ReturnStmt.h"
#include "AST/StringLiteral.h"
#include "AST/TypeRef.h"
#include "AST/UnaryOperation.h"

#include "Basic/SourceManager.h"

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
        auto parseFieldExpr(Lex::Token Token, AST::Expr *LHS) noexcept
            -> AST::FieldExpr *;

        [[nodiscard]]
        auto parseCallExpr(Lex::Token NameToken, Lex::Token ParenToken) noexcept
            -> AST::CallExpr *;

        [[nodiscard]] auto
        parseArraySubscriptExpr(AST::Expr *Lhs, Lex::Token BracketToken) noexcept
            -> AST::ArraySubscriptExpr *;

        [[nodiscard]]
        auto parseIdentifierForLhs(Lex::Token Token) noexcept -> AST::Expr *;

        [[nodiscard]] auto parseLhs() noexcept -> AST::Expr *;

        [[nodiscard]]
        auto parseBinOpRhs(AST::Expr *LHS, uint64_t MinPrec) noexcept
            -> AST::Expr *;

        [[nodiscard]]
        auto parseIfStmt(Lex::Token IfToken) noexcept -> AST::IfStmt *;

        [[nodiscard]]
        auto parseReturnStmt(Lex::Token ReturnToken) noexcept
            -> AST::ReturnStmt *;

        [[nodiscard]]
        auto parseCompoundStmt(Lex::Token CurlyToken) noexcept
            -> AST::CompoundStmt *;

        [[nodiscard]] auto parseStmt() noexcept -> AST::Stmt *;
        [[nodiscard]]
        auto parseExpression(bool ExprIsOptional = false) noexcept
            -> std::optional<AST::Expr *>;

        [[nodiscard]]
        auto parseExpressionAndEnd(bool ExprIsOptional = false) noexcept
            -> AST::Expr *;

        [[nodiscard]] auto parseTypeQualifiers() noexcept
            -> std::optional<Sema::TypeQualifiers>;

        [[nodiscard]] auto
        parseTypeExprInstList(
            std::vector<AST::TypeRef::Inst *> &InstList) noexcept -> bool;

        [[nodiscard]] auto parseTypeExpr() noexcept -> AST::TypeRef *;
        [[nodiscard]]
        auto parseVarQualifiers() noexcept -> std::optional<AST::VarQualifiers>;

        [[nodiscard]]
        auto parseVarDecl(Lex::Token Token) noexcept -> AST::VarDecl *;

        [[nodiscard]] auto
        parseArrayDecl(Lex::Token LeftBracketLoc) noexcept -> AST::ArrayDecl *;

        [[nodiscard]] auto parseFuncDecl() noexcept -> AST::LvalueNamedDecl *;

        [[nodiscard]] auto
        parseFieldList(Lex::Token NameToken,
                       std::vector<AST::FieldDecl *> &FieldList) noexcept
            -> bool;

        [[nodiscard]] auto
        parseEnumMemberList(
            std::vector<AST::EnumMemberDecl *> &FieldList) noexcept -> bool;

        [[nodiscard]] auto parseEnumDecl() noexcept -> AST::LvalueNamedDecl *;
        [[nodiscard]] auto parseStructDecl() noexcept -> AST::LvalueNamedDecl *;

        [[nodiscard]] auto peek() const noexcept -> std::optional<Lex::Token>;
        [[nodiscard]] auto prev() const noexcept -> std::optional<Lex::Token>;
        [[nodiscard]]
        auto current() const noexcept -> std::optional<Lex::Token>;

        [[nodiscard]] auto peekIs(Lex::TokenKind Kind) const noexcept -> bool;
        [[nodiscard]]
        auto peekIsOneOf(
            const std::initializer_list<Lex::TokenKind> &Kinds) const noexcept
                -> bool;

        auto consumeIfToken(Lex::TokenKind Kind) -> std::optional<Lex::Token>;

        [[nodiscard]]
        auto tokenContent(Lex::Token Token) const noexcept -> std::string_view;

        [[nodiscard]]
        auto tokenKeyword(Lex::Token Token) const noexcept -> Lex::Keyword;

        auto consume(uint64_t Skip = 0) -> std::optional<Lex::Token>;
        auto expect(Lex::TokenKind Kind, bool Optional = false) -> bool;
        auto getCurrOrPrevLoc() const noexcept -> SourceLocation;

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
        auto parseTopLevelExpressionOrStmt() noexcept -> AST::Stmt *;

        [[nodiscard]] constexpr const auto &getTokenList() const noexcept {
            return TokenList;
        }

        [[nodiscard]] constexpr auto position() const noexcept {
            return Index;
        }
    };
}
