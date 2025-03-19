/*
 * AST/ArraySubscriptExpr.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "Expr.h"

namespace AST {
    struct ArraySubscriptExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::ArraySubscriptExpr;
    protected:
        SourceLocation BracketLoc;

        Expr *Base;
        std::vector<Stmt *> DetailList;
    public:
        constexpr explicit
        ArraySubscriptExpr(const SourceLocation BracketLoc,
                           Expr *const Base,
                           std::vector<Stmt *> &&DetailList) noexcept
        : Expr(ObjKind), BracketLoc(BracketLoc), Base(Base),
          DetailList(std::move(DetailList)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getBracketLoc() const noexcept {
            return this->BracketLoc;
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->getBracketLoc();
        }

        [[nodiscard]] constexpr auto getBase() const noexcept {
            return this->Base;
        }

        [[nodiscard]] constexpr auto getDetailList() const noexcept {
            return std::span(this->DetailList);
        }

        [[nodiscard]] constexpr auto &getDetailListRef() noexcept {
            return this->DetailList;
        }

        constexpr auto setBase(Expr *const Base) noexcept -> decltype(*this) {
            this->Base = Base;
            return *this;
        }
    };
}
