/*
 * AST/Node.h
 */

#pragma once

#include "AST/ExprKind.h"
#include "llvm/IR/IRBuilder.h"

namespace AST {
    struct Expr {
    protected:
        ExprKind Kind = ExprKind::Base;
        constexpr explicit Expr(const ExprKind Kind) noexcept : Kind(Kind) {}
    public:
        virtual ~Expr() noexcept = default;

        [[nodiscard]] constexpr auto getKind() const noexcept {
            return Kind;
        }

        [[nodiscard]] virtual llvm::Value *codegen() noexcept = 0;
    };
}