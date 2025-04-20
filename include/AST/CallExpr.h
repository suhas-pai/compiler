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
        struct Argument {
            std::optional<std::string_view> Name;
            Expr *Expr;

            constexpr
            Argument(const std::optional<std::string_view> Name,
                     struct Expr *const Expr) noexcept
            : Name(Name), Expr(Expr) {}
        };

        constexpr static auto ObjKind = NodeKind::CallExpr;
    protected:
        Expr *CalleeExpr;

        SourceLocation ParenLoc;
        Qualifiers Quals;

        std::vector<Argument> ArgList;
    public:
        explicit
        CallExpr(Expr *const Callee,
                 const SourceLocation ParenLoc,
                 const Qualifiers &Quals,
                 const std::span<Argument> ArgList) noexcept
        : Expr(ObjKind), CalleeExpr(Callee), ParenLoc(ParenLoc), Quals(Quals),
          ArgList(ArgList.begin(), ArgList.end()) {}

        explicit
        CallExpr(Expr *const Callee,
                 const SourceLocation ParenLoc,
                 const Qualifiers &Quals,
                 std::vector<Argument> &&ArgList) noexcept
        : Expr(ObjKind), CalleeExpr(Callee), ParenLoc(ParenLoc), Quals(Quals),
          ArgList(std::move(ArgList)) {}

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
            return std::span(this->ArgList);
        }

        [[nodiscard]] constexpr auto &getArgsRef() noexcept {
            return this->ArgList;
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
