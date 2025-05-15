/*
 * AST/Decls/InlineArrayParamVarDecl.h
 * © suhas pai
 */

#pragma once
#include "AST/Decls/ParamVarDecl.h"

namespace AST {
    struct InlineArrayParamVarDecl : public ParamVarDecl {
    public:
        constexpr static auto ObjKind = NodeKind::InlineArrayParamVarDecl;

        constexpr explicit
        InlineArrayParamVarDecl(const std::string_view Name,
                                const SourceLocation NameLoc,
                                Expr *const TypeExpr,
                                Expr *const DefaultExpr) noexcept
        : ParamVarDecl(ObjKind, Name, NameLoc, TypeExpr, DefaultExpr) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Stmt) noexcept {
            return IsOfKind(*Stmt);
        }
    };
}
