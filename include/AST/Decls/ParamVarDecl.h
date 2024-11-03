/*
 * AST/Decls/ParamVarDecl.h
 * © suhas pai
 */

#pragma once
#include "AST/Decls/LvalueTypedDecl.h"

namespace AST {
    struct ParamVarDecl : public LvalueTypedDecl {
    public:
        constexpr explicit
        ParamVarDecl(const std::string_view Name,
                     const SourceLocation NameLoc,
                     TypeRef *const TypeRef,
                     Expr *const DefaultExpr) noexcept
        : LvalueTypedDecl(ObjKind, Name, NameLoc, DefaultExpr, TypeRef) {}

        constexpr explicit
        ParamVarDecl(const std::string_view Name,
                     const SourceLocation NameLoc,
                     Sema::Type *const Type,
                     Expr *const DefaultExpr) noexcept
        : LvalueTypedDecl(ObjKind, Name, NameLoc, DefaultExpr, Type)  {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getDefaultExpr() const noexcept {
            return this->getRvalueExpr();
        }
    };
}
