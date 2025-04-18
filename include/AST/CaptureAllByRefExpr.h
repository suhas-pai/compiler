/*
 * AST/CaptureAllByRefExpr.h
 * © suhas pai
 */

#pragma once

#include "AST/Expr.h"
#include "AST/Qualifiers.h"

namespace AST {
    struct CaptureAllByRefExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::CaptureAllByRefExpr;
    protected:
        SourceLocation Loc;
        Qualifiers Qualifiers;
    public:
        explicit
        CaptureAllByRefExpr(const SourceLocation Loc,
                            const struct Qualifiers &Qualifiers) noexcept
        : Expr(ObjKind), Loc(Loc), Qualifiers(Qualifiers) {}

        explicit
        CaptureAllByRefExpr(const SourceLocation Loc,
                            struct Qualifiers &&Qualifiers) noexcept
        : Expr(ObjKind), Loc(Loc), Qualifiers(std::move(Qualifiers)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->Loc;
        }

        [[nodiscard]] constexpr auto &getQualifiers() const noexcept {
            return this->Qualifiers;
        }

        [[nodiscard]] constexpr auto &getQualifiersRef() noexcept {
            return this->Qualifiers;
        }
    };
}
