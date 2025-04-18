/*
 * AST/OptionalUnwrapExpr.h
 * © suhas pai
 */

#pragma once
#include "AST/Expr.h"

namespace AST {
    struct OptionalUnwrapExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::OptionalUnwrapExpr;
    protected:
        SourceLocation Loc;
        Expr *Base;
    public:
        constexpr explicit
        OptionalUnwrapExpr(const SourceLocation Loc, Expr *const Base) noexcept
        : Expr(ObjKind), Loc(Loc), Base(Base) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getBase() const noexcept {
            return this->Base;
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->Loc;
        }

        constexpr auto setBase(Expr *const Base) noexcept -> decltype(*this) {
            this->Base = Base;
            return *this;
        }

        constexpr auto setLoc(const SourceLocation Loc) noexcept
            -> decltype(*this)
        {
            this->Loc = Loc;
            return *this;
        }
    };
}
