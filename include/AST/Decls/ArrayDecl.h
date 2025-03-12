/*
 * AST/Decls/ArrayDecl.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Expr.h"
#include "Source/SourceLocation.h"

namespace AST {
    class ArrayDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::ArrayDecl;
    protected:
        SourceLocation LeftBracketLoc;
        std::vector<Stmt *> ElementList;
    public:
        constexpr explicit
        ArrayDecl(const SourceLocation LeftBracketLoc,
                  const std::span<Stmt *> ElementList) noexcept
        : Expr(ObjKind), LeftBracketLoc(LeftBracketLoc),
          ElementList(ElementList.begin(), ElementList.end()) {}

        constexpr explicit
        ArrayDecl(const SourceLocation LeftBracketLoc,
                  std::vector<Stmt *> &&ElementList) noexcept
        : Expr(ObjKind), LeftBracketLoc(LeftBracketLoc),
          ElementList(std::move(ElementList)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getLeftBracketLoc() const noexcept {
            return this->LeftBracketLoc;
        }

        [[nodiscard]] SourceLocation getLoc() const noexcept override {
            return this->getLeftBracketLoc();
        }

        [[nodiscard]] constexpr auto getElementList() const noexcept {
            return std::span(this->ElementList);
        }

        [[nodiscard]] constexpr auto &getElementListRef() noexcept {
            return this->ElementList;
        }

        constexpr auto setLeftBracketLoc(const SourceLocation Loc) noexcept
            -> decltype(*this)
        {
            this->LeftBracketLoc = Loc;
            return *this;
        }
    };
}
