/*
 * AST/Decls/NamedDecl.h
 * © suhas pai
 */

#pragma once
#include <string>

#include "Basic/SourceLocation.h"
#include "Base.h"

namespace AST {
    struct NamedDecl : public Decl {
    protected:
        std::string Name;
        SourceLocation NameLoc;

        constexpr explicit
        NamedDecl(const NodeKind Kind,
                  const std::string_view Name,
                  const SourceLocation NameLoc) noexcept
        : Decl(Kind), Name(Name), NameLoc(NameLoc) {}
    public:
        [[nodiscard]]
        constexpr static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() >= NodeKind::NamedDeclBase
                && Stmt.getKind() <= NodeKind::NamedDeclLast;
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

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return NameLoc;
        }

        constexpr auto setName(const std::string_view Name) noexcept
            -> decltype(*this)
        {
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