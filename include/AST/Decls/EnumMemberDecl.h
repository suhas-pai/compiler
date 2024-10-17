/*
 * AST/Decls/EnumDecl.h
 * © suhas pai
 */

#pragma once

#include "AST/Decls/LvalueNamedDecl.h"
#include "AST/Expr.h"

namespace AST {
    struct EnumMemberDecl : public LvalueNamedDecl {
    public:
        constexpr static auto ObjKind = NodeKind::EnumMemberDecl;
    protected:
        Expr *InitExpr;
    public:
        constexpr explicit
        EnumMemberDecl(const std::string_view Name,
                       const SourceLocation NameLoc,
                       Expr *const InitExpr) noexcept
        : LvalueNamedDecl(ObjKind, Name, NameLoc, InitExpr) {}

        [[nodiscard]]
        constexpr static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getInitExpr() const noexcept {
            return getRvalueExpr();
        }

        constexpr auto setInitExpr(Expr *const InitExpr) noexcept
            -> decltype(*this)
        {
            setRvalueExpr(InitExpr);
            return *this;
        }
    };
}
