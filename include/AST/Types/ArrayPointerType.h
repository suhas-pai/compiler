/*
 * AST/Types/PointerTypeExpr.h
 * © suhas pai
 */

 #pragma once

#include "AST/Expr.h"
#include "AST/Qualifiers.h"

namespace AST {
    struct ArrayPointerTypeExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::PointerType;
    protected:
        SourceLocation Loc;
        Qualifiers Quals;
    public:
        explicit
        ArrayPointerTypeExpr(const SourceLocation Loc,
                             const Qualifiers &Quals) noexcept
        : Expr(ObjKind), Loc(Loc), Quals(Quals){}

        explicit
        ArrayPointerTypeExpr(const SourceLocation Loc,
                             Qualifiers &&Quals) noexcept
        : Expr(ObjKind), Loc(Loc), Quals(std::move(Quals)){}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] inline auto &getQualifiers() const noexcept {
            return this->Quals;
        }

        [[nodiscard]] inline auto &getQualifiersRef() noexcept {
            return this->Quals;
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->Loc;
        }
    };
}
