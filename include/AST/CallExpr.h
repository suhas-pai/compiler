/*
 * AST/CallExpr.h
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Qualifiers.h"
#include "Expr.h"

namespace AST {
    struct CallExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::CallExpr;
    protected:
        Expr *CalleeExpr;

        SourceLocation ParenLoc;
        Qualifiers Quals;

        std::vector<Expr *> Args;
    public:
        explicit
        CallExpr(Expr *const Callee,
                 const SourceLocation ParenLoc,
                 const Qualifiers &Quals,
                 const std::span<Expr *> Args) noexcept
        : Expr(ObjKind), CalleeExpr(Callee), ParenLoc(ParenLoc), Quals(Quals),
          Args(Args.begin(), Args.end()) {}

        explicit
        CallExpr(Expr *const Callee,
                 const SourceLocation ParenLoc,
                 const Qualifiers &Quals,
                 std::vector<Expr *> &&Args) noexcept
        : Expr(ObjKind), CalleeExpr(Callee), ParenLoc(ParenLoc), Quals(Quals),
          Args(std::move(Args)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getCalleeExpr() const noexcept {
            return this->CalleeExpr;
        }

        [[nodiscard]] constexpr auto getParenLoc() const noexcept {
            return this->ParenLoc;
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->getParenLoc();
        }

        [[nodiscard]] constexpr auto getArgs() const noexcept {
            return std::span(this->Args);
        }

        [[nodiscard]] constexpr auto &getArgsRef() noexcept {
            return this->Args;
        }

        constexpr auto setCallee(Expr *const Callee) noexcept -> decltype(*this)
        {
            this->CalleeExpr = Callee;
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
