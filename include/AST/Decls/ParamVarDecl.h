/*
 * AST/Decls/ParamVarDecl.h
 * © suhas pai
 */

#pragma once
#include "AST/Decls/LvalueTypedDecl.h"

namespace AST {
    struct ParamVarDecl : public LvalueTypedDecl {
    public:
        constexpr static auto ObjKind = NodeKind::ParamVarDecl;
    public:
        constexpr explicit
        ParamVarDecl(const std::string_view Name,
                     const SourceLocation NameLoc,
                     TypeRef *const TypeRef) noexcept
        : LvalueTypedDecl(ObjKind, Name, NameLoc, /*RvalueExpr=*/nullptr,
                          TypeRef) {}

        constexpr explicit
        ParamVarDecl(const std::string_view Name,
                     const SourceLocation NameLoc,
                     Sema::Type *const Type) noexcept
        : LvalueTypedDecl(ObjKind, Name, NameLoc, /*RvalueExpr=*/nullptr,
                          Type)  {}

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
