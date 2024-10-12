/*
 * AST/ArraySubscriptExpr.h
 * © suhas pai
 */

#pragma once

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct ArraySubscriptExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::ArraySubscriptExpr;
    protected:
        SourceLocation BracketLoc;

        Expr *Base;
        Expr *BracketedExpr;
    public:
        constexpr explicit
        ArraySubscriptExpr(const SourceLocation BracketLoc,
                           Expr *const Base,
                           Expr *const BracketExpr) noexcept
        : Expr(ObjKind), BracketLoc(BracketLoc), Base(Base),
          BracketedExpr(BracketExpr) {}

        [[nodiscard]]
        constexpr static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getBracketLoc() const noexcept {
            return BracketLoc;
        }

        [[nodiscard]] constexpr auto getBase() const noexcept {
            return Base;
        }

        [[nodiscard]] constexpr auto getBracketedExpr() const noexcept {
            return BracketedExpr;
        }

        constexpr auto setBase(Expr *const Base) noexcept -> decltype(*this) {
            this->Base = Base;
            return *this;
        }

        constexpr auto setBracketedExpr(Expr *const BracketedExpr) noexcept
            -> decltype(*this)
        {
            this->BracketedExpr = BracketedExpr;
            return *this;
        }
    };
}
