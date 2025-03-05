/*
 * AST/Decls/DeclStmt.h
 * © suhas pai
 */

#pragma once
#include "AST/Stmt.h"

namespace AST {
    struct DeclStmt : public Stmt {
    protected:
        constexpr explicit DeclStmt(const NodeKind Kind) noexcept
        : Stmt(Kind) {}
    public:
        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() >= NodeKind::DeclStmtBase &&
                   Stmt.getKind() <= NodeKind::DeclStmtLast;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }
    };
}
