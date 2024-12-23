/*
 * AST/IfStmt.h
 */

#pragma once

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct IfStmt : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::IfStmt;
    protected:
        SourceLocation IfLoc;

        Expr *Cond;
        Stmt *Then;
        Stmt *Else;
    public:
        constexpr explicit
        IfStmt(const SourceLocation IfLoc,
               Expr *const Cond,
               Stmt *const Then,
               Stmt *const Else) noexcept
        : Expr(ObjKind), IfLoc(IfLoc), Cond(Cond), Then(Then), Else(Else) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getIfLoc() const noexcept {
            return IfLoc;
        }

        [[nodiscard]] constexpr auto getCond() const noexcept {
            return Cond;
        }

        [[nodiscard]] constexpr auto getThen() const noexcept {
            return Then;
        }

        [[nodiscard]] constexpr auto getElse() const noexcept {
            return Else;
        }

        constexpr auto setIfLoc(const SourceLocation IfLoc) noexcept
            -> decltype(*this)
        {
            this->IfLoc = IfLoc;
            return *this;
        }

        constexpr auto setCond(Expr *const Cond) noexcept -> decltype(*this) {
            this->Cond = Cond;
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
