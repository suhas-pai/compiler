/*
 * AST/FunctionCall.h
 */

#pragma once

#include <string>
#include <vector>

#include "Basic/SourceLocation.h"
#include "Expr.h"

namespace AST {
    struct FunctionCall : Expr {
    public:
        constexpr static auto ObjKind = NodeKind::FunctionCall;
    protected:
        SourceLocation NameLoc;
        std::string Name;

        std::vector<Expr *> Args;
    public:
        constexpr explicit
        FunctionCall(const SourceLocation NameLoc,
                     const std::string_view Name,
                     const std::vector<Expr *> &Args) noexcept
        : Expr(ObjKind), NameLoc(NameLoc), Name(Name), Args(Args) {}

        constexpr explicit
        FunctionCall(const SourceLocation NameLoc,
                     const std::string_view Name,
                     std::vector<Expr *> &&Args) noexcept
        : Expr(ObjKind), NameLoc(NameLoc), Name(Name), Args(Args) {}

        constexpr explicit FunctionCall(const std::string_view Name) noexcept
        : Expr(ObjKind), Name(Name) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getNameLoc() const noexcept {
            return NameLoc;
        }

        [[nodiscard]]
        constexpr auto getName() const noexcept -> std::string_view {
            return Name;
        }

        [[nodiscard]] constexpr auto &getArgs() const noexcept {
            return Args;
        }

        [[nodiscard]] constexpr auto &getArgsRef() noexcept {
            return Args;
        }
    };
}