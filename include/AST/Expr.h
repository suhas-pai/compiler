/*
 * AST/Node.h
 */

#pragma once

#include "AST/ExprKind.h"
#include "Backend/LLVM/Handler.h"
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

        virtual
        llvm::Value *codegen(Backend::LLVM::Handler &Handler) noexcept = 0;
    };
}