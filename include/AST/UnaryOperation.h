/*
 * AST/UnaryOperation.h
 */

#pragma once

#include "AST/Expr.h"
#include "AST/ExprKind.h"
#include "Basic/SourceLocation.h"
#include "Lex/Token.h"
#include "Parse/Operator.h"

namespace AST {
    struct UnaryOperation : Expr {
    protected:
        SourceLocation Loc;
        Parse::UnaryOperator Operator;
        Expr *Operand;
    public:
        constexpr explicit
        UnaryOperation(const SourceLocation Loc,
                       const Parse::UnaryOperator Operator,
                       Expr *const Operand = nullptr) noexcept
        : Expr(ExprKind::UnaryOperation), Loc(Loc), Operator(Operator),
          Operand(Operand) {}

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return Loc;
        }

        [[nodiscard]] constexpr auto getOperator() const noexcept {
            return Operator;
        }

        [[nodiscard]] constexpr auto getOperand() const noexcept {
            return Operand;
        }

        constexpr auto setOperator(const Parse::UnaryOperator Operator) noexcept
            -> decltype(*this)
        {
            this->Operator = Operator;
            return *this;
        }

        constexpr auto setOperand(Expr *const Operand) noexcept
            -> decltype(*this)
        {
            this->Operand = Operand;
            return *this;
        }

        [[nodiscard]] llvm::Value *codegen() noexcept override;
    };
}