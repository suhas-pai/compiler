/*
 * AST/Node.h
 */

#pragma once
#include "Stmt.h"

namespace AST {
    struct Expr : public Stmt {
    public:
        constexpr static auto ObjKind = NodeKind::Base;
    protected:
        constexpr explicit Expr(const NodeKind Kind) noexcept : Stmt(Kind) {}
    public:
        virtual ~Expr() noexcept = default;
    };
}