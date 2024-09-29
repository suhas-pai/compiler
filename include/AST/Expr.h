/*
 * AST/Node.h
 */

#pragma once
#include "Stmt.h"

namespace AST {
    struct Expr : public Stmt {
    protected:
        constexpr explicit Expr(const NodeKind Kind) noexcept : Stmt(Kind) {}
    };
}