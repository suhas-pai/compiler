/*
 * AST/Stmt.h
 */

#pragma once
#include "NodeKind.h"

namespace AST {
    struct Stmt {
    private:
        NodeKind Kind;
    protected:
        constexpr explicit Stmt(const NodeKind Kind) noexcept : Kind(Kind) {}
    public:
        [[nodiscard]] constexpr auto getKind() const noexcept {
            return this->Kind;
        }
    };
}