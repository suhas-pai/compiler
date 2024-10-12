/*
 * AST/BinaryOperation.h
 */

#pragma once

#include "AST/Expr.h"
#include "Parse/Operator.h"

namespace AST {
    struct BinaryOperation : Expr {
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
                        Expr *const Lhs,
                        Expr *const Rhs) noexcept
        : Expr(ObjKind), Operator(Operator), Loc(Loc), Lhs(Lhs), Rhs(Rhs) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return Loc;
        }

        [[nodiscard]] constexpr auto getOperator() const noexcept {
            return Operator;
        }

        [[nodiscard]] constexpr auto &getLhs() const noexcept {
            return *Lhs;
        }

        [[nodiscard]] constexpr auto &getRhs() const noexcept {
            return *Rhs;
        }

        constexpr auto setLhs(Expr *const Lhs) noexcept -> decltype(*this) {
            this->Lhs = Lhs;
            return *this;
        }

        constexpr auto setRhs(Expr *const Rhs) noexcept -> decltype(*this) {
            this->Rhs = Rhs;
            return *this;
        }
    };
}