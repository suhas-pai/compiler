/*
 * AST/Node.h
 */

#pragma once

#include "AST/ExprKind.h"
#include "Backend/LLVM/Handler.h"
#include "llvm/IR/IRBuilder.h"

namespace AST {
    struct Expr {
    public:
        constexpr static auto ObjKind = ExprKind::Base;
    protected:
        ExprKind Kind = ExprKind::Base;
        constexpr explicit Expr(const ExprKind Kind) noexcept : Kind(Kind) {}
    public:
        virtual ~Expr() noexcept = default;

        [[nodiscard]] constexpr auto getKind() const noexcept {
            return Kind;
        }

        [[nodiscard]] static inline auto IsOfKind(const Expr &Expr) noexcept {
            return (Expr.getKind() == ObjKind);
        }

        [[nodiscard]]
        static inline auto classof(const Expr *const Obj) noexcept {
            return IsOfKind(*Obj);
        }

        virtual
        llvm::Value *codegen(Backend::LLVM::Handler &Handler) noexcept = 0;
    };
}