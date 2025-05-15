/*
 * AST/DerefExpr.h
 * © suhas pai
 */

#pragma once
#include "Expr.h"

namespace AST {
    struct DerefExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::DerefExpr;
    protected:
        SourceLocation Loc;
        Expr *Operand = nullptr;
    public:
        constexpr explicit
        DerefExpr(const SourceLocation Loc, Expr *const Operand) noexcept
        : Expr(ObjKind), Operand(Operand) {}

        [[nodiscard]] static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]] static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getOperand() const noexcept {
            return this->Operand;
        }

        [[nodiscard]] virtual SourceLocation getLoc() const noexcept override {
            return Operand->getLoc();
        }

        constexpr auto setOperand(Expr &Operand) noexcept -> decltype(*this) {
            this->Operand = &Operand;
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
