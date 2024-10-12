/*
 * AST/StructDecl.h
 * © suhas pai
 */

#pragma once

#include <vector>
#include "FieldDecl.h"

namespace AST {
    struct StructDecl : public NamedDecl {
    public:
        constexpr static auto ObjKind = NodeKind::StructDecl;
    protected:
        std::vector<FieldDecl *> FieldList;
    public:
        constexpr explicit
        StructDecl(const std::string_view Name,
                   const SourceLocation NameLoc,
                   const std::vector<FieldDecl *> &FieldList) noexcept
        : NamedDecl(ObjKind, Name, NameLoc), FieldList(FieldList) {}

        constexpr explicit
        StructDecl(const std::string_view Name,
                   const SourceLocation NameLoc,
                   std::vector<FieldDecl *> &&FieldList) noexcept
        : NamedDecl(ObjKind, Name, NameLoc), FieldList(std::move(FieldList)) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto &getFieldList() const noexcept {
            return FieldList;
        }

        [[nodiscard]] constexpr auto &getFieldListRef() noexcept {
            return FieldList;
        }
    };
}
