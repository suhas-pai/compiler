/*
 * AST/Decls/ArrayBindingParamVarDecl.h
 * © suhas pai
 */

#pragma once
#include "AST/Decls/ArrayBindingVarDecl.h"

namespace AST {
    struct ArrayBindingParamVarDecl : public ArrayBindingVarDecl {
    public:
        constexpr static auto ObjKind = NodeKind::ArrayBindingParamVarDecl;
    protected:
    public:
        explicit
        ArrayBindingParamVarDecl(const SourceLocation Loc,
                                 const struct Qualifiers &Qualifiers,
                                 const std::span<ArrayBindingItem *> ItemList,
                                 Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Qualifiers, ItemList, InitExpr) {}

        explicit
        ArrayBindingParamVarDecl(const SourceLocation Loc,
                                 const struct Qualifiers &Qualifiers,
                                 std::vector<ArrayBindingItem *> &&ItemList,
                                 Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Qualifiers, ItemList, InitExpr) {}

        explicit
        ArrayBindingParamVarDecl(const SourceLocation Loc,
                                 struct Qualifiers &&Qualifiers,
                                 const std::span<ArrayBindingItem *> &ItemList,
                                 Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Qualifiers, ItemList, InitExpr) {}

        explicit
        ArrayBindingParamVarDecl(const SourceLocation Loc,
                                 struct Qualifiers &&Qualifiers,
                                 std::vector<ArrayBindingItem *> &&ItemList,
                                 Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Qualifiers, ItemList, InitExpr) {}

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
