/*
 * AST/CastExpr.h
 * © suhas pai
 */

#pragma once

#include "AST/Expr.h"
#include "Source/SourceLocation.h"

namespace AST {
    struct CastExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::CastExpr;
    protected:
        SourceLocation AsLoc;

        Expr *Operand;
        Expr *TypeExpr;
    public:
        constexpr explicit
        CastExpr(const SourceLocation AsLoc,
                 Expr *const Operand,
                 Expr *const TypeExpr) noexcept
        : Expr(ObjKind), AsLoc(AsLoc), Operand(Operand), TypeExpr(TypeExpr) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return this->AsLoc;
        }

        [[nodiscard]] constexpr auto getOperand() const noexcept {
            return this->Operand;
        }

        [[nodiscard]] constexpr auto getTypeExpr() const noexcept {
            return this->TypeExpr;
        }

        constexpr auto setAsLoc(const SourceLocation AsLoc) noexcept
            -> decltype(*this)
        {
            this->AsLoc = AsLoc;
            return *this;
        }

        constexpr auto setOperand(Expr *const Operand) noexcept
            -> decltype(*this)
        {
            this->Operand = Operand;
            return *this;
        }

        constexpr auto setTypeExpr(Expr *const TypeExpr) noexcept
            -> decltype(*this)
        {
            this->TypeExpr = TypeExpr;
            return *this;
        }
    };
}
