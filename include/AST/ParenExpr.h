/*
 * AST/ParenExpr.h
 */

#pragma once
#include "Expr.h"

namespace AST {
    struct ParenExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::ParenExpr;
    protected:
        SourceLocation Loc;
        Expr *ChildExpr;
    public:
        constexpr explicit
        ParenExpr(const SourceLocation Loc,
                  Expr *const ChildExpr = nullptr) noexcept
        : Expr(ObjKind), Loc(Loc), ChildExpr(ChildExpr) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->Loc;
        }

        [[nodiscard]] constexpr auto getChildExpr() const noexcept {
            return this->ChildExpr;
        }

        constexpr auto setChildExpr(Expr *const ChildExpr) noexcept
            -> decltype(*this)
        {
            this->ChildExpr = ChildExpr;
            return *this;
        }
    };
}