/*
 * AST/CallExpr.h
 */

#pragma once

#include <string>
#include <vector>

#include "Basic/SourceLocation.h"
#include "Expr.h"

namespace AST {
    struct CallExpr : Expr {
    public:
        constexpr static auto ObjKind = NodeKind::CallExpr;
    protected:
        std::string Name;
        SourceLocation NameLoc;

        std::vector<Expr *> Args;
    public:
        constexpr explicit
        CallExpr(const std::string_view Name,
                 const SourceLocation NameLoc,
                 const std::vector<Expr *> &Args) noexcept
        : Expr(ObjKind), Name(Name), NameLoc(NameLoc), Args(Args) {}

        constexpr explicit
        CallExpr(const std::string_view Name,
                 const SourceLocation NameLoc,
                 std::vector<Expr *> &&Args) noexcept
        : Expr(ObjKind), Name(Name), NameLoc(NameLoc), Args(std::move(Args)) {}

        [[nodiscard]]
        constexpr static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]]
        constexpr auto getName() const noexcept -> std::string_view {
            return Name;
        }

        [[nodiscard]] constexpr auto getNameLoc() const noexcept {
            return NameLoc;
        }

        [[nodiscard]] constexpr auto &getArgs() const noexcept {
            return Args;
        }

        [[nodiscard]] constexpr auto &getArgsRef() noexcept {
            return Args;
        }

        constexpr auto setName(std::string &&Name) noexcept -> decltype(*this) {
            this->Name = std::move(Name);
            return *this;
        }

        constexpr auto
        setName(const std::string_view Name) noexcept -> decltype(*this) {
            this->Name = Name;
            return *this;
        }

        constexpr auto setNameLoc(const SourceLocation NameLoc) noexcept
            -> decltype(*this)
        {
            this->NameLoc = NameLoc;
            return *this;
        }
    };
}