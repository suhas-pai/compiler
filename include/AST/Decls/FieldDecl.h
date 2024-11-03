/*
 * AST/Decls/FieldDecl.h
 * © suhas pai
 */

#pragma once
#include "LvalueTypedDecl.h"

namespace AST {
    struct FieldDecl : public LvalueTypedDecl {
    public:
        constexpr static auto ObjKind = NodeKind::FieldDecl;
    public:
        constexpr explicit
        FieldDecl(const std::string_view Name,
                  const SourceLocation NameLoc,
                  TypeRef *const Type,
                  Expr *const InitExpr) noexcept
        : LvalueTypedDecl(ObjKind, Name, NameLoc, InitExpr, Type) {}

        constexpr explicit
        FieldDecl(const std::string_view Name,
                  const SourceLocation NameLoc,
                  Sema::Type *const Type,
                  Expr *const InitExpr) noexcept
        : LvalueTypedDecl(ObjKind, Name, NameLoc, InitExpr, Type) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getInitExpr() const noexcept {
            return getRvalueExpr();
        }
    };
}
