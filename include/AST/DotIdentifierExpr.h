/*
 * AST/DotIdentifierExpr.h
 * © suhas pai
 */

#pragma once

#include <string>

#include "Expr.h"
#include "Qualifiers.h"


namespace AST {
    struct DotIdentifierExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::DotIdentifierExpr;
    private:
        SourceLocation DotLoc;

        Qualifiers Quals;
        std::string Identifier;
    public:
        explicit
        DotIdentifierExpr(const SourceLocation DotLoc,
                          const Qualifiers &Quals,
                          const std::string_view Identifier) noexcept
        : Expr(ObjKind), DotLoc(DotLoc), Quals(Quals),
          Identifier(Identifier) {}

        explicit
        DotIdentifierExpr(const SourceLocation DotLoc,
                          const Qualifiers &Quals,
                          std::string &&Identifier) noexcept
        : Expr(ObjKind), DotLoc(DotLoc), Quals(Quals),
          Identifier(std::move(Identifier)) {}

        explicit
        DotIdentifierExpr(const SourceLocation DotLoc,
                          Qualifiers &&Quals,
                          const std::string_view Identifier) noexcept
        : Expr(ObjKind), DotLoc(DotLoc), Quals(std::move(Quals)),
          Identifier(Identifier) {}

        explicit
        DotIdentifierExpr(const SourceLocation DotLoc,
                          Qualifiers &&Quals,
                          std::string &&Identifier) noexcept
        : Expr(ObjKind), DotLoc(DotLoc), Quals(std::move(Quals)),
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
            return this->Quals;
        }

        [[nodiscard]] auto &getQualifiersRef() noexcept {
            return this->Quals;
        }
    };
}
