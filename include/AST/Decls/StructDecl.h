/*
 * AST/Decls/StructDecl.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Expr.h"

namespace AST {
    struct StructDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::StructDecl;
    protected:
        std::vector<Stmt *> FieldList;
    public:
        constexpr explicit
        StructDecl(const std::span<Stmt *> FieldList) noexcept
        : Expr(ObjKind),
          FieldList(std::vector(FieldList.begin(), FieldList.end())) {}

        constexpr explicit StructDecl(std::vector<Stmt *> &&FieldList) noexcept
        : Expr(ObjKind), FieldList(std::move(FieldList)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getFieldList() const noexcept {
            return std::span(this->FieldList);
        }

        [[nodiscard]] constexpr auto &getFieldListRef() noexcept {
            return this->FieldList;
        }
    };
}
