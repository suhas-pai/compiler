/*
 * AST/CaptureAllByValueExpr.h
 * © suhas pai
 */

#pragma once

#include "Expr.h"
#include "Qualifiers.h"

namespace AST {
    struct CaptureAllByValueExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::CaptureAllByValueExpr;
    protected:
        SourceLocation Loc;
        Qualifiers Quals;
    public:
        explicit
        CaptureAllByValueExpr(const SourceLocation Loc,
                              const Qualifiers &Quals) noexcept
        : Expr(ObjKind), Loc(Loc), Quals(Quals) {}

        explicit
        CaptureAllByValueExpr(const SourceLocation Loc,
                              Qualifiers &&Quals) noexcept
        : Expr(ObjKind), Loc(Loc), Quals(std::move(Quals)) {}

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
            return this->Quals;
        }

        [[nodiscard]] constexpr auto &getQualifiersRef() noexcept {
            return this->Quals;
        }
    };
}
