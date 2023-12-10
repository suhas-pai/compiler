/*
 * AST/Stmt.h
 */

#pragma once
#include "AST/NodeKind.h"

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
    };
}