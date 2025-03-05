/*
 * AST/Types/PointerType.h
 * © suhas pai
 */

#pragma once

#include "AST/Expr.h"
#include "Sema/Types/PointerQualifiers.h"
#include "Source/SourceLocation.h"

namespace AST {
    struct PointerType : public Expr {
    protected:
        SourceLocation StarLoc;
        Expr *Base;
        Sema::PointerBaseTypeQualifiers Qualifiers;
    public:
        constexpr explicit
        PointerType(const SourceLocation StarLoc,
                    Expr *const Base,
                    const Sema::PointerBaseTypeQualifiers Qualifiers) noexcept
        : Expr(NodeKind::PointerType), StarLoc(StarLoc), Base(Base),
          Qualifiers(Qualifiers) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == NodeKind::PointerType;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getStarLoc() const noexcept {
            return this->StarLoc;
        }

        [[nodiscard]] constexpr auto getBase() const noexcept {
            return this->Base;
        }

        [[nodiscard]] constexpr auto getQualifiers() const noexcept {
            return this->Qualifiers;
        }

        [[nodiscard]] constexpr auto &getQualifiersRef() noexcept {
            return this->Qualifiers;
        }

        constexpr auto setStarLoc(const SourceLocation StarLoc) noexcept
            -> decltype(*this)
        {
            this->StarLoc = StarLoc;
            return *this;
        }

        constexpr auto setBase(Expr *const Base) noexcept -> decltype(*this) {
            this->Base = Base;
            return *this;
        }
    };
}
