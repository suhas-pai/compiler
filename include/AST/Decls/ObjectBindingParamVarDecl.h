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
    protected:
    public:
        explicit
        ObjectBindingParamVarDecl(
            const SourceLocation Loc,
            const struct Qualifiers &Qualifiers,
            const std::span<ObjectBindingField *> ItemList,
            Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(ObjKind, Loc, Qualifiers, ItemList, InitExpr) {}

        explicit
        ObjectBindingParamVarDecl(const SourceLocation Loc,
                                  const struct Qualifiers &Qualifiers,
                                  std::vector<ObjectBindingField *> &&ItemList,
                                  Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(ObjKind, Loc, Qualifiers, ItemList, InitExpr) {}

        explicit
        ObjectBindingParamVarDecl(
            const SourceLocation Loc,
            struct Qualifiers &&Qualifiers,
            const std::span<ObjectBindingField *> &ItemList,
            Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(ObjKind, Loc, Qualifiers, ItemList, InitExpr) {}

        explicit
        ObjectBindingParamVarDecl(const SourceLocation Loc,
                                  struct Qualifiers &&Qualifiers,
                                  std::vector<ObjectBindingField *> &&ItemList,
                                  Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(ObjKind, Loc, Qualifiers, ItemList, InitExpr) {}

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
