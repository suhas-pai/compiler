/*
 * AST/Stmt.h
 */

#pragma once
#include "AST/Expr.h"

namespace AST {
    struct Stmt : public Expr {
    protected:
        constexpr explicit Stmt(const ExprKind Kind) noexcept : Expr(Kind) {}
    };
}