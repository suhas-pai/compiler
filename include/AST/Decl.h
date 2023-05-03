/*
 * include/AST/Decl.h
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
        virtual auto getName() const noexcept -> std::string_view = 0;

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return (Node->getKind() >= NodeKind::DeclBase &&
                    Node->getKind() <= NodeKind::DeclLast);
        }
    };
}