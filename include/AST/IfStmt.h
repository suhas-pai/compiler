/*
 * AST/IfStmt.h
 */

#pragma once
#include "AST/Expr.h"
#include "Backend/LLVM/Handler.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct IfStmt : public Stmt {
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
        : Stmt(ObjKind), IfLoc(IfLoc), Cond(Cond), Then(Then), Else(Else) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return (Stmt.getKind() == ObjKind);
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
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

        constexpr auto setCond(Expr *const Condition) noexcept
            -> decltype(*this)
        {
            this->Cond = Condition;
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

        [[nodiscard]] llvm::Value *
        codegen(Backend::LLVM::Handler &Handler,
                llvm::IRBuilder<> &Builder,
                Backend::LLVM::ValueMap &ValueMap) noexcept override;
    };
}
