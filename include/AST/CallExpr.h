/*
 * AST/CallExpr.h
 */

#pragma once
#include <vector>

#include "Basic/SourceLocation.h"
#include "Expr.h"

namespace AST {
    struct CallExpr : Expr {
    public:
        constexpr static auto ObjKind = NodeKind::CallExpr;
    protected:
        AST::Expr *Callee;
        SourceLocation ParenLoc;

        std::vector<Expr *> Args;
    public:
        constexpr explicit
        CallExpr(AST::Expr *const Callee,
                 const SourceLocation ParenLoc,
                 const std::vector<Expr *> &Args) noexcept
        : Expr(ObjKind), Callee(Callee), ParenLoc(ParenLoc), Args(Args) {}

        [[nodiscard]]
        constexpr static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getCallee() const noexcept {
            return this->Callee;
        }

        [[nodiscard]] constexpr auto getParenLoc() const noexcept {
            return ParenLoc;
        }

        [[nodiscard]] constexpr auto &getArgs() const noexcept {
            return Args;
        }

        [[nodiscard]] constexpr auto &getArgsRef() noexcept {
            return Args;
        }

        constexpr auto setCallee(AST::Expr *const Callee) noexcept
            -> decltype(*this)
        {
            this->Callee = Callee;
            return *this;
        }

        constexpr auto setParenLoc(const SourceLocation ParenLoc) noexcept
            -> decltype(*this)
        {
            this->ParenLoc = ParenLoc;
            return *this;
        }
    };
}