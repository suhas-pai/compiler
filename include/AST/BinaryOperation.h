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
        SourceLocation Loc;
        Parse::BinaryOperator Operator;

        Expr *Lhs = nullptr;
        Expr *Rhs = nullptr;
    public:
        constexpr explicit
        BinaryOperation(const SourceLocation Loc,
                        const Parse::BinaryOperator Operator,
                        Expr *const Lhs,
                        Expr *const Rhs) noexcept
        : Expr(ObjKind), Loc(Loc), Operator(Operator), Lhs(Lhs), Rhs(Rhs) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return (Stmt.getKind() == ObjKind);
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

        [[nodiscard]] constexpr auto getLhs() const noexcept {
            return Lhs;
        }

        [[nodiscard]] constexpr auto getRhs() const noexcept {
            return Rhs;
        }

        constexpr auto setLhs(Expr *const Lhs) noexcept -> decltype(*this) {
            this->Lhs = Lhs;
            return *this;
        }

        constexpr auto setRhs(Expr *const Rhs) noexcept -> decltype(*this) {
            this->Rhs = Rhs;
            return *this;
        }

        [[nodiscard]] std::optional<llvm::Value *>
        codegen(Backend::LLVM::Handler &Handler,
                llvm::IRBuilder<> &Builder,
                Backend::LLVM::ValueMap &ValueMap) noexcept;
    };
}