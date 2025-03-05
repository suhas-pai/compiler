/*
 * AST/Decls/EnumDecl.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Expr.h"
#include "AST/Stmt.h"

namespace AST {
    class EnumDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::EnumDecl;
    protected:
        std::vector<Stmt *> MemberList;
    public:
        constexpr explicit EnumDecl(const std::span<Stmt *> MemberList) noexcept
        : Expr(ObjKind),
          MemberList(std::vector(MemberList.begin(), MemberList.end())) {}

        constexpr explicit EnumDecl(std::vector<Stmt *> &&MemberList) noexcept
        : Expr(ObjKind), MemberList(std::move(MemberList)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getMemberList() const noexcept {
            return std::span(this->MemberList);
        }

        [[nodiscard]] constexpr auto &getMemberListRef() noexcept {
            return this->MemberList;
        }
    };
}
