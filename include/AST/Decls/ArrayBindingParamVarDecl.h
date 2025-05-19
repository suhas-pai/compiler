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

        explicit
        ArrayBindingParamVarDecl(const SourceLocation Loc,
                                 const Qualifiers &Quals,
                                 const std::span<ArrayBindingItem *> ItemList,
                                 Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Quals, ItemList, InitExpr) {}

        explicit
        ArrayBindingParamVarDecl(const SourceLocation Loc,
                                 const Qualifiers &Quals,
                                 std::vector<ArrayBindingItem *> &&ItemList,
                                 Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Quals, ItemList, InitExpr) {}

        explicit
        ArrayBindingParamVarDecl(const SourceLocation Loc,
                                 Qualifiers &&Quals,
                                 const std::span<ArrayBindingItem *> &ItemList,
                                 Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Quals, ItemList, InitExpr) {}

        explicit
        ArrayBindingParamVarDecl(const SourceLocation Loc,
                                 Qualifiers &&Quals,
                                 std::vector<ArrayBindingItem *> &&ItemList,
                                 Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Quals, ItemList, InitExpr) {}

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
