/*
 * AST/ParenExpr.h
 */

#pragma once

#include "AST/Expr.h"
#include "Lex/Token.h"

namespace AST {
    struct ParenExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::Paren;
    protected:
        SourceLocation Loc;
        SourceLocation End;

        Expr *ChildExpr;
    public:
        constexpr explicit
        ParenExpr(const Lex::Token Token,
                  Expr *const ChildExpr = nullptr) noexcept
        : Expr(ObjKind), Loc(Token.Loc), End(Token.End), ChildExpr(ChildExpr) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return (Stmt.getKind() == ObjKind);
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return Loc;
        }

        [[nodiscard]] constexpr auto getEnd() const noexcept {
            return End;
        }

        [[nodiscard]] constexpr auto getChildExpr() const noexcept {
            return ChildExpr;
        }

        constexpr auto setChildExpr(Expr *const ChildExpr) noexcept
            -> decltype(*this)
        {
            this->ChildExpr = ChildExpr;
            return *this;
        }

        [[nodiscard]] std::optional<llvm::Value *>
        codegen(Backend::LLVM::Handler &Handler,
                llvm::IRBuilder<> &Builder,
                Backend::LLVM::ValueMap &ValueMap) noexcept;
    };
}