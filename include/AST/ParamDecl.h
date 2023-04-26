/*
 * AST/ParamDecl.h
 */

#pragma once

#include <string>

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct ParamDecl : public Expr {
    protected:
        SourceLocation NameLoc;
        std::string Name;
    public:
        constexpr explicit
        ParamDecl(const SourceLocation NameLoc,
                  const std::string_view Name) noexcept
        : Expr(ExprKind::Parameter), NameLoc(NameLoc), Name(Name) {}

        constexpr explicit
        ParamDecl(const SourceLocation NameLoc, std::string &&Name) noexcept
        : Expr(ExprKind::Parameter), NameLoc(NameLoc), Name(Name) {}

        [[nodiscard]] constexpr auto getName() const noexcept {
            return std::string_view(Name);
        }

        [[nodiscard]] constexpr auto getNameLoc() const noexcept {
            return std::string_view(Name);
        }

        constexpr auto setName(const std::string_view Name) noexcept
            -> decltype(*this)
        {
            this->Name = std::string_view(Name);
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

        llvm::Value *codegen(Backend::LLVM::Handler &Handler) noexcept override;
    };
}