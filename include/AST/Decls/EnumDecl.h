/*
 * AST/Decls/EnumDecl.h
 * © suhas pai
 */

#pragma once

#include <vector>

#include "AST/Decls/EnumMemberDecl.h"
#include "AST/Decls/NamedDecl.h"

namespace AST {
    class EnumDecl : public NamedDecl {
    public:
        constexpr static auto ObjKind = NodeKind::EnumDecl;
    protected:
        std::vector<EnumMemberDecl *> MemberList;
    public:
        constexpr explicit
        EnumDecl(const std::string_view Name,
                 const SourceLocation NameLoc,
                 const std::vector<EnumMemberDecl *> &MemberList) noexcept
        : NamedDecl(ObjKind, Name, NameLoc), MemberList(MemberList) {}

        constexpr explicit
        EnumDecl(const std::string_view Name,
                 const SourceLocation NameLoc,
                 std::vector<EnumMemberDecl *> &&MemberList) noexcept
        : NamedDecl(ObjKind, Name, NameLoc),
          MemberList(std::move(MemberList)) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
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