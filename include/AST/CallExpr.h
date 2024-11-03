/*
 * AST/CallExpr.h
 */

#pragma once
#include <vector>

#include "Basic/SourceLocation.h"
#include "Expr.h"

namespace AST {
    struct CallExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::CallExpr;

        struct Qualifiers {
        protected:
            bool IsInline : 1 = false;
            bool IsComptime : 1 = false;
            bool IsNoInline : 1 = false;
        public:
            constexpr explicit Qualifiers() noexcept = default;

            [[nodiscard]] constexpr auto isInline() const noexcept {
                return IsInline;
            }

            [[nodiscard]] constexpr auto isComptime() const noexcept {
                return IsComptime;
            }

            [[nodiscard]] constexpr auto isNoInline() const noexcept {
                return IsNoInline;
            }

            constexpr auto setIsInline(const bool IsInline) noexcept
                -> decltype(*this)
            {
                this->IsInline = IsInline;
                return *this;
            }

            constexpr auto setIsComptime(const bool IsComptime) noexcept
                -> decltype(*this)
            {
                this->IsComptime = IsComptime;
                return *this;
            }

            constexpr auto setIsNoInline(const bool IsNoInline) noexcept
                -> decltype(*this)
            {
                this->IsNoInline = IsNoInline;
                return *this;
            }
        };
    protected:
        Expr *Callee;

        SourceLocation ParenLoc;
        Qualifiers Quals;

        std::vector<Expr *> Args;
    public:
        constexpr explicit
        CallExpr(Expr *const Callee,
                 const SourceLocation ParenLoc,
                 const Qualifiers Quals,
                 const std::vector<Expr *> &Args) noexcept
        : Expr(ObjKind), Callee(Callee), ParenLoc(ParenLoc), Quals(Quals),
          Args(Args) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getCallee() const noexcept {
            return Callee;
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

        constexpr auto setCallee(Expr *const Callee) noexcept -> decltype(*this)
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