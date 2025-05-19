/*
 * AST/Decls/ObjectBindingParamVarDecl.h
 * © suhas pai
 */

#pragma once
#include "AST/Decls/ObjectBindingVarDecl.h"

namespace AST {
    struct ObjectBindingParamVarDecl : public ObjectBindingVarDecl {
    public:
        constexpr static auto ObjKind = NodeKind::ObjectBindingParamVarDecl;

        explicit
        ObjectBindingParamVarDecl(
            const SourceLocation Loc,
            const Qualifiers &Quals,
            const std::span<ObjectBindingField *> ItemList,
            Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(ObjKind, Loc, Quals, ItemList, InitExpr) {}

        explicit
        ObjectBindingParamVarDecl(const SourceLocation Loc,
                                  const Qualifiers &Quals,
                                  std::vector<ObjectBindingField *> &&ItemList,
                                  Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(ObjKind, Loc, Quals, ItemList, InitExpr) {}

        explicit
        ObjectBindingParamVarDecl(
            const SourceLocation Loc,
            Qualifiers &&Quals,
            const std::span<ObjectBindingField *> &ItemList,
            Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(ObjKind, Loc, Quals, ItemList, InitExpr) {}

        explicit
        ObjectBindingParamVarDecl(const SourceLocation Loc,
                                  Qualifiers &&Quals,
                                  std::vector<ObjectBindingField *> &&ItemList,
                                  Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(ObjKind, Loc, Quals, ItemList, InitExpr) {}

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
