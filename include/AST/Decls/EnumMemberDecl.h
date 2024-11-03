/*
 * AST/Decls/EnumDecl.h
 * © suhas pai
 */

#pragma once

#include "AST/Expr.h"
#include "LvalueNamedDecl.h"

namespace AST {
    struct EnumMemberDecl : public LvalueNamedDecl {
    public:
        constexpr static auto ObjKind = NodeKind::EnumMemberDecl;
    public:
        constexpr explicit
        EnumMemberDecl(const std::string_view Name,
                       const SourceLocation NameLoc,
                       Expr *const InitExpr) noexcept
        : LvalueNamedDecl(ObjKind, Name, NameLoc, InitExpr) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
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
