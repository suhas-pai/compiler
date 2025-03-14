/*
 * AST/Expr.h
 */

#pragma once

#include "Source/SourceLocation.h"
#include "Stmt.h"

namespace AST {
    struct Expr : public Stmt {
    protected:
        constexpr explicit Expr(const NodeKind Kind) noexcept : Stmt(Kind) {}
    public:
        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() >= NodeKind::ExprBase &&
                   Stmt.getKind() <= NodeKind::ExprLast;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        virtual SourceLocation getLoc() const noexcept = 0;
    };
}