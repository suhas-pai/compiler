/*
 * AST/Decls/EnumDecl.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Expr.h"

namespace AST {
    class EnumDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::EnumDecl;
    protected:
        SourceLocation Loc;
        std::vector<Stmt *> MemberList;
    public:
        constexpr explicit
        EnumDecl(const SourceLocation Loc,
                 const std::span<Stmt *> MemberList) noexcept
        : Expr(ObjKind), Loc(Loc),
          MemberList(MemberList.begin(), MemberList.end()) {}

        constexpr explicit
        EnumDecl(const SourceLocation Loc,
                 std::vector<Stmt *> &&MemberList) noexcept
        : Expr(ObjKind), Loc(Loc), MemberList(std::move(MemberList)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->Loc;
        }

        [[nodiscard]] constexpr auto getMemberList() const noexcept {
            return std::span(this->MemberList);
        }

        [[nodiscard]] constexpr auto &getMemberListRef() noexcept {
            return this->MemberList;
        }
    };
}
