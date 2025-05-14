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
        constexpr static auto ObjKind = NodeKind::ArrayPointerType;
    protected:
        SourceLocation Loc;
        Qualifiers Quals;

        Expr *Base;
    public:
        explicit
        ArrayPointerTypeExpr(const SourceLocation Loc,
                             const Qualifiers &Quals,
                             Expr *const Base) noexcept
        : Expr(ObjKind), Loc(Loc), Quals(Quals), Base(Base) {}

        explicit
        ArrayPointerTypeExpr(const SourceLocation Loc,
                             Qualifiers &&Quals,
                             Expr *const Base) noexcept
        : Expr(ObjKind), Loc(Loc), Quals(std::move(Quals)), Base(Base) {}

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

        [[nodiscard]] constexpr auto getBase() const noexcept {
            return this->Base;
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->Loc;
        }

        constexpr auto setBase(Expr *const Base) noexcept
            -> decltype(*this)
        {
            this->Base = Base;
            return *this;
        }
    };
}
