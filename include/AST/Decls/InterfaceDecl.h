/*
 * AST/Decls/InterfaceDecl.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Decls/FieldDecl.h"
#include "AST/Expr.h"

namespace AST {
    struct InterfaceDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::InterfaceDecl;
    protected:
        SourceLocation Loc;
        std::vector<Stmt *> FieldList;
    public:
        constexpr explicit
        InterfaceDecl(const SourceLocation Loc,
                      const std::span<Stmt *> FieldList) noexcept
        : Expr(ObjKind), Loc(Loc),
          FieldList(FieldList.begin(), FieldList.end()) {}

        constexpr explicit
        InterfaceDecl(const SourceLocation Loc,
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
