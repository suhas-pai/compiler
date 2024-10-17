/*
 * AST/ArrayExpr.h
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
                  std::vector<Expr *> &&ElementList) noexcept
        : Expr(ObjKind), ElementList(ElementList) {}

        constexpr explicit
        ArrayDecl(const SourceLocation LeftBracketLoc,
                  const std::vector<Expr *> &ElementList) noexcept
        : Expr(ObjKind), ElementList(ElementList) {}

        [[nodiscard]]
        constexpr static inline auto IsOfKind(const Stmt &Node) noexcept {
            return Node.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] inline auto getLeftBracketLoc() const noexcept {
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
