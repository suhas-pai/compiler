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

        constexpr explicit
        ParamVarDecl(const std::string_view Name,
                     const SourceLocation NameLoc,
                     Expr *const TypeExpr,
                     Expr *const DefaultExpr) noexcept
        : LvalueTypedDecl(ObjKind, Name, NameLoc, TypeExpr, DefaultExpr) {}

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

        constexpr auto setDefaultExpr(Expr *const DefaultExpr) noexcept
            -> decltype(*this)
        {
            this->setRvalueExpr(DefaultExpr);
            return *this;
        }
    };
}
