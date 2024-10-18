/*
 * AST/Decls/FieldDecl.h
 * © suhas pai
 */

#pragma once
#include "AST/Decls/LvalueTypedDecl.h"

namespace AST {
    struct FieldDecl : public LvalueTypedDecl {
    public:
        constexpr static auto ObjKind = NodeKind::FieldDecl;
    public:
        constexpr explicit
        FieldDecl(const std::string_view Name,
                  const SourceLocation NameLoc,
                  TypeRef *const Type) noexcept
        : LvalueTypedDecl(ObjKind, Name, NameLoc, /*RvalueExpr=*/nullptr,
                          Type) {}

        constexpr explicit
        FieldDecl(const std::string_view Name,
                  const SourceLocation NameLoc,
                  Sema::Type *const Type) noexcept
        : LvalueTypedDecl(ObjKind, Name, NameLoc, /*RvalueExpr=*/nullptr,
                          Type) {}

        [[nodiscard]]
        constexpr static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }
    };
}
