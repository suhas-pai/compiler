/*
 * AST/Decls/ArrayDecl.h
 * © suhas pai
 */

#pragma once
#include <vector>

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"

namespace AST {
    class ArrayDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::ArrayDecl;
    protected:
        SourceLocation LeftBracketLoc;
        std::vector<Expr *> ElementList;
    public:
        constexpr explicit
        ArrayDecl(const SourceLocation LeftBracketLoc,
                  const std::vector<Expr *> &ElementList) noexcept
        : Expr(ObjKind), LeftBracketLoc(LeftBracketLoc),
          ElementList(ElementList) {}

        constexpr explicit
        ArrayDecl(const SourceLocation LeftBracketLoc,
                  std::vector<Expr *> &&ElementList) noexcept
        : Expr(ObjKind), LeftBracketLoc(LeftBracketLoc),
          ElementList(ElementList) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getLeftBracketLoc() const noexcept {
            return LeftBracketLoc;
        }

        [[nodiscard]] constexpr auto &getElementList() const noexcept {
            return ElementList;
        }

        [[nodiscard]] constexpr auto &getElementListRef() noexcept {
            return ElementList;
        }

        constexpr auto setLeftBracketLoc(const SourceLocation Loc) noexcept
            -> decltype(*this)
        {
            this->LeftBracketLoc = Loc;
            return *this;
        }
    };
}
