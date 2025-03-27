/*
 * AST/Decls/OptionalFieldDecl.h
 * © suhas pai
 */

#pragma once
#include "FieldDecl.h"

namespace AST {
    struct OptionalFieldDecl : public FieldDecl {
    public:
        constexpr static auto ObjKind = NodeKind::OptionalFieldDecl;

        constexpr explicit
        OptionalFieldDecl(const std::string_view Name,
                          const SourceLocation NameLoc,
                          Expr *const TypeExpr,
                          Expr *const InitExpr) noexcept
        : FieldDecl(ObjKind, Name, NameLoc, TypeExpr, InitExpr) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }
    };
}
