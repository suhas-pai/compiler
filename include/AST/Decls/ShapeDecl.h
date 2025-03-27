/*
 * AST/Decls/ShapeDecl.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Expr.h"

namespace AST {
    struct ShapeDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::ShapeDecl;
    protected:
        SourceLocation Loc;
        std::vector<Stmt *> FieldList;
    public:
        constexpr explicit
        ShapeDecl(const SourceLocation Loc,
                  const std::span<Stmt *> FieldList) noexcept
        : Expr(ObjKind), Loc(Loc),
          FieldList(FieldList.begin(), FieldList.end()) {}

        constexpr explicit
        ShapeDecl(const SourceLocation Loc,
                  std::vector<Stmt *> &&FieldList) noexcept
        : Expr(ObjKind), Loc(Loc), FieldList(std::move(FieldList)) {}

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

        [[nodiscard]] constexpr auto getFieldList() const noexcept {
            return std::span(this->FieldList);
        }

        [[nodiscard]] constexpr auto &getFieldListRef() noexcept {
            return this->FieldList;
        }
    };
}
