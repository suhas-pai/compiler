/*
 * AST/DeclRefExpr.h
 */

#pragma once

#include <string>
#include "Expr.h"

namespace AST {
    struct DeclRefExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::DeclRefExpr;
    protected:
        std::string Name;
        SourceLocation NameLoc;
    public:
        constexpr explicit
        DeclRefExpr(const std::string_view Name,
                    const SourceLocation NameLoc) noexcept
        : Expr(ObjKind), Name(Name), NameLoc(NameLoc) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getNameLoc() const noexcept {
            return this->NameLoc;
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->getNameLoc();
        }

        [[nodiscard]]
        constexpr auto getName() const noexcept -> std::string_view {
            return this->Name;
        }

        constexpr auto setName(const std::string_view Name) noexcept
            -> decltype(*this)
        {
            this->Name = Name;
            return *this;
        }

        constexpr auto setName(std::string &&Name) noexcept -> decltype(*this) {
            this->Name = std::move(Name);
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
