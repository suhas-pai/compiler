/*
 * AST/IfExpr.h
 */

#pragma once
#include "Expr.h"

namespace AST {
    struct IfExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::IfStmt;
    protected:
        SourceLocation IfLoc;

        Expr *Cond;
        Stmt *Then;
        Stmt *Else;
    public:
        constexpr explicit
        IfExpr(const SourceLocation IfLoc,
               Expr &Cond,
               Stmt *const Then,
               Stmt *const Else) noexcept
        : Expr(ObjKind), IfLoc(IfLoc), Cond(&Cond), Then(Then), Else(Else) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getIfLoc() const noexcept {
            return this->IfLoc;
        }

        [[nodiscard]] SourceLocation getLoc() const noexcept override {
            return this->getIfLoc();
        }

        [[nodiscard]] constexpr auto getCond() const noexcept {
            return this->Cond;
        }

        [[nodiscard]] constexpr auto getThen() const noexcept {
            return this->Then;
        }

        [[nodiscard]] constexpr auto getElse() const noexcept {
            return this->Else;
        }

        constexpr auto setIfLoc(const SourceLocation IfLoc) noexcept
            -> decltype(*this)
        {
            this->IfLoc = IfLoc;
            return *this;
        }

        constexpr auto setCond(Expr &Cond) noexcept -> decltype(*this) {
            this->Cond = &Cond;
            return *this;
        }

        constexpr auto setThen(Stmt *const Then) noexcept -> decltype(*this) {
            this->Then = Then;
            return *this;
        }

        constexpr auto setElse(Stmt *const Else) noexcept -> decltype(*this) {
            this->Else = Else;
            return *this;
        }
    };
}
