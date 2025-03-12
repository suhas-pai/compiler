/*
 * AST/BinaryOperation.h
 */

#pragma once

#include "Parse/Operator.h"
#include "Expr.h"

namespace AST {
    struct BinaryOperation : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::BinaryOperation;
    protected:
        Parse::BinaryOperator Operator;
        SourceLocation Loc;

        Expr *Lhs;
        Expr *Rhs;
    public:
        constexpr explicit
        BinaryOperation(const Parse::BinaryOperator Operator,
                        const SourceLocation Loc,
                        Expr &Lhs,
                        Expr &Rhs) noexcept
        : Expr(ObjKind), Operator(Operator), Loc(Loc), Lhs(&Lhs), Rhs(&Rhs) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] SourceLocation getLoc() const noexcept override {
            return this->Loc;
        }

        [[nodiscard]] constexpr auto getOperator() const noexcept {
            return this->Operator;
        }

        [[nodiscard]] constexpr auto &getLhs() const noexcept {
            return *this->Lhs;
        }

        [[nodiscard]] constexpr auto &getRhs() const noexcept {
            return *this->Rhs;
        }

        [[nodiscard]] constexpr auto isAssignmentOperation() const noexcept {
            return BinaryOperatorIsAssigment(this->getOperator());
        }

        [[nodiscard]] constexpr auto isComparableOperation() const noexcept {
            return BinaryOperatorIsComparable(this->getOperator());
        }

        [[nodiscard]] constexpr auto isArithmeticOperation() const noexcept {
            return BinaryOperatorIsArithmetic(this->getOperator());
        }

        [[nodiscard]] constexpr auto isLogicalOperation() const noexcept {
            return BinaryOperatorIsLogical(this->getOperator());
        }

        [[nodiscard]] constexpr auto isBitwiseOperation() const noexcept {
            return BinaryOperatorIsBitwise(this->getOperator());
        }

        [[nodiscard]] constexpr auto isShiftOperation() const noexcept {
            return BinaryOperatorIsShift(this->getOperator());
        }

        constexpr auto setLhs(Expr &Lhs) noexcept -> decltype(*this) {
            this->Lhs = &Lhs;
            return *this;
        }

        constexpr auto setRhs(Expr &Rhs) noexcept -> decltype(*this) {
            this->Rhs = &Rhs;
            return *this;
        }
    };
}
