/*
 * AST/Decls/EnumDecl.h
 * © suhas pai
 */

#pragma once

#include "AST/Decls/NamedDecl.h"
#include "AST/Expr.h"

namespace AST {
    struct EnumMemberDecl : public NamedDecl {
    public:
        constexpr static auto ObjKind = NodeKind::EnumMemberDecl;
    protected:
        Expr *InitExpr;
    public:
        constexpr explicit
        EnumMemberDecl(const std::string_view Name,
                       const SourceLocation NameLoc,
                       Expr *const InitExpr) noexcept
        : NamedDecl(ObjKind, Name, NameLoc), InitExpr(InitExpr) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getInitExpr() const noexcept {
            return InitExpr;
        }

        constexpr auto setInitExpr(Expr *const InitExpr) noexcept
            -> decltype(*this)
        {
            this->InitExpr = InitExpr;
            return *this;
        }
    };
}
