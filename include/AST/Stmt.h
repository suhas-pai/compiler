/*
 * AST/Stmt.h
 */

#pragma once

#include "AST/NodeKind.h"
#include "Backend/LLVM/Handler.h"

namespace AST {
    struct Stmt {
    private:
        NodeKind Kind;
    protected:
        constexpr explicit Stmt(const NodeKind Kind) noexcept : Kind(Kind) {}
    public:
        [[nodiscard]] constexpr auto getKind() const noexcept {
            return Kind;
        }

        virtual llvm::Value *
        codegen(Backend::LLVM::Handler &Handler,
                Backend::LLVM::ValueMap &ValueMap) noexcept = 0;
    };
}