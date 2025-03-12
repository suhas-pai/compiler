/*
 * AST/Types/OptionalType.h
 * © suhas pai
 */

 #pragma once

 #include "AST/Expr.h"
 #include "Source/SourceLocation.h"

namespace AST {
    struct OptionalTypeExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::OptionalType;
    protected:
        SourceLocation Loc;
        Expr *Operand;
    public:
        constexpr explicit
        OptionalTypeExpr(const SourceLocation Loc, Expr *const Operand) noexcept
        : Expr(ObjKind), Loc(Loc), Operand(Operand) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getOperand() const noexcept {
            return this->Operand;
        }

        [[nodiscard]] SourceLocation getLoc() const noexcept override {
            return this->Loc;
        }

        constexpr auto setOperand(Expr &Operand) noexcept -> decltype(*this) {
            this->Operand = &Operand;
            return *this;
        }
    };
}
