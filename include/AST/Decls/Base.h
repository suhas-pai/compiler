/*
 * AST/Decls/Base.h
 */

#pragma once

#include "AST/Stmt.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct Decl : public Stmt {
    protected:
        constexpr explicit Decl(const NodeKind Kind) noexcept : Stmt(Kind) {}
    public:
        [[nodiscard]]
        constexpr static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() >= NodeKind::NamedDeclBase
                && Stmt.getKind() <= NodeKind::NamedDeclLast;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Stmt *const Node) noexcept {
            return Node->getKind() >= NodeKind::DeclBase
                && Node->getKind() <= NodeKind::DeclLast;
        }

        [[nodiscard]] virtual SourceLocation getLoc() const noexcept = 0;
    };
}
