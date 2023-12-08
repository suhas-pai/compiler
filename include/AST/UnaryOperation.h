/*
 * AST/UnaryOperation.h
 */

#pragma once

#include "AST/Expr.h"
#include "Parse/Operator.h"

namespace AST {
    struct UnaryOperation : Expr {
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

        [[nodiscard]] constexpr auto &getOperand() const noexcept {
            return *Operand;
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

        [[nodiscard]] std::optional<llvm::Value *>
        codegen(Backend::LLVM::Handler &Handler,
                llvm::IRBuilder<> &Builder,
                Backend::LLVM::ValueMap &ValueMap) noexcept;
    };
}