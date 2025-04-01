/*
 * AST/DotIdentifierExpr.h
 * © suhas pai
 */

#pragma once
#include <string>

#include "AST/Expr.h"
#include "AST/Qualifiers.h"
#include "Source/SourceLocation.h"

namespace AST {
    struct DotIdentifierExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::DotIdentifierExpr;

    private:
        SourceLocation DotLoc;
        AST::Qualifiers Qualifiers;

        std::string Identifier;
    public:
        explicit
        DotIdentifierExpr(const SourceLocation DotLoc,
                          AST::Qualifiers &&Qualifiers,
                          const std::string_view Identifier) noexcept
        : Expr(ObjKind), DotLoc(DotLoc), Qualifiers(std::move(Qualifiers)),
          Identifier(Identifier) {}

        explicit
        DotIdentifierExpr(const SourceLocation DotLoc,
                          AST::Qualifiers &&Qualifiers,
                          std::string &&Identifier) noexcept
        : Expr(ObjKind), DotLoc(DotLoc), Qualifiers(std::move(Qualifiers)),
          Identifier(std::move(Identifier)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getDotLoc() const noexcept {
            return this->DotLoc;
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->getDotLoc();
        }

        [[nodiscard]] constexpr auto getIdentifier() const noexcept {
            return this->Identifier;
        }

        [[nodiscard]] auto &getQualifiers() const noexcept {
            return this->Qualifiers;
        }

        [[nodiscard]] auto &getQualifiersRef() noexcept {
            return this->Qualifiers;
        }
    };
}
