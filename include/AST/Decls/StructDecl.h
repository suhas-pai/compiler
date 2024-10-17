/*
 * AST/StructDecl.h
 * © suhas pai
 */

#pragma once

#include <vector>

#include "AST/Expr.h"
#include "FieldDecl.h"

namespace AST {
    struct StructDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::StructDecl;
    protected:
        std::vector<FieldDecl *> FieldList;
    public:
        constexpr explicit
        StructDecl(const std::vector<FieldDecl *> &FieldList) noexcept
        : Expr(ObjKind), FieldList(FieldList) {}

        constexpr explicit
        StructDecl(std::vector<FieldDecl *> &&FieldList) noexcept
        : Expr(ObjKind), FieldList(std::move(FieldList)) {}

        [[nodiscard]]
        constexpr static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Stmt *const Node) noexcept {
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
