/*
 * AST/Decls/FieldDecl.h
 * © suhas pai
 */

#pragma once
#include "LvalueTypedDecl.h"

namespace AST {
    struct FieldDecl : public LvalueTypedDecl {
    public:
        constexpr static auto ObjKind = NodeKind::FieldDecl;
    protected:
        constexpr explicit
        FieldDecl(const NodeKind ObjKind,
                  const std::string_view Name,
                  const SourceLocation NameLoc,
                  Expr *const TypeExpr,
                  Expr *const InitExpr) noexcept
        : LvalueTypedDecl(ObjKind, Name, NameLoc, TypeExpr, InitExpr) {}
    public:
        constexpr explicit
        FieldDecl(const std::string_view Name,
                  const SourceLocation NameLoc,
                  Expr *const TypeExpr,
                  Expr *const InitExpr) noexcept
        : FieldDecl(ObjKind, Name, NameLoc, TypeExpr, InitExpr) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind ||
                   Stmt.getKind() == NodeKind::OptionalFieldDecl;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getInitExpr() const noexcept {
            return this->getRvalueExpr();
        }

        constexpr auto setInitExpr(Expr *const InitExpr) noexcept
            -> decltype(*this)
        {
            this->setRvalueExpr(InitExpr);
            return *this;
        }
    };
}
