/*
 * AST/Decls/EnumDecl.h
 * © suhas pai
 */

#pragma once
#include <vector>

#include "AST/Expr.h"
#include "EnumMemberDecl.h"

namespace AST {
    class EnumDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::EnumDecl;
    protected:
        std::vector<EnumMemberDecl *> MemberList;
    public:
        constexpr explicit
        EnumDecl(const std::vector<EnumMemberDecl *> &MemberList) noexcept
        : Expr(ObjKind), MemberList(MemberList) {}

        constexpr explicit
        EnumDecl(std::vector<EnumMemberDecl *> &&MemberList) noexcept
        : Expr(ObjKind), MemberList(std::move(MemberList)) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto &getMemberList() const noexcept {
            return MemberList;
        }

        [[nodiscard]] constexpr auto &getMemberListRef() noexcept {
            return MemberList;
        }
    };
}
