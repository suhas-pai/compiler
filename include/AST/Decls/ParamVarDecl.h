/*
 * AST/ParamVarDecl.h
 * © suhas pai
 */

#pragma once
#include "AST/Decls/ValueDecl.h"

namespace AST {
    struct ParamVarDecl : public ValueDecl {
    public:
        constexpr static auto ObjKind = NodeKind::ParamVarDecl;
    public:
        constexpr explicit
        ParamVarDecl(const std::string_view Name,
                     const SourceLocation NameLoc,
                     TypeRef *const TypeRef) noexcept
        : ValueDecl(ObjKind, Name, NameLoc, Linkage::Private, TypeRef) {}

        constexpr explicit
        ParamVarDecl(const std::string_view Name,
                     const SourceLocation NameLoc,
                     Sema::Type *const Type) noexcept
        : ValueDecl(ObjKind, Name, NameLoc, Linkage::Private, Type)  {}

        [[nodiscard]]
        static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }
    };
}
