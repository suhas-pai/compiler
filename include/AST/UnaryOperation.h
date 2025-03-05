/*
 * AST/UnaryOperation.h
 */

#pragma once

#include "Parse/Operator.h"
#include "Expr.h"

namespace AST {
    struct UnaryOperation : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::UnaryOperation;
    protected:
        SourceLocation Loc;
        Parse::UnaryOperator Operator;

        Expr *Operand;
    public:
        constexpr explicit
        UnaryOperation(const SourceLocation Loc,
                       const Parse::UnaryOperator Operator,
                       Expr *const Operand = nullptr) noexcept
        : Expr(ObjKind), Loc(Loc), Operator(Operator), Operand(Operand) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return this->Loc;
        }

        [[nodiscard]] constexpr auto getOperator() const noexcept {
            return this->Operator;
        }

        [[nodiscard]] constexpr auto &getOperand() const noexcept {
            return *this->Operand;
        }

        constexpr auto setOperator(const Parse::UnaryOperator Operator) noexcept
            -> decltype(*this)
        {
            this->Operator = Operator;
            return *this;
        }

        constexpr auto setOperand(Expr &Operand) noexcept -> decltype(*this) {
            this->Operand = &Operand;
            return *this;
        }
    };
}