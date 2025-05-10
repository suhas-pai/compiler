/*
 * AST/Types/ArrayType.h
 * © suhas pai
 */

#pragma once

#include "AST/Expr.h"
#include "AST/Qualifiers.h"

namespace AST {
    struct ArrayTypeExpr : public Expr {
    public:
        constexpr static auto NodeKind = NodeKind::ArrayType;
    protected:
        SourceLocation BracketLoc;

        Expr *SizeExpr;
        Expr *ConstraintExpr;

        Expr *Base;
        Qualifiers Qualifiers;
    public:
        explicit
        ArrayTypeExpr(const SourceLocation BracketLoc,
                      Expr *const Size,
                      Expr *const Constraint,
                      Expr *const Base,
                      const struct Qualifiers &Qualifiers) noexcept
        : Expr(NodeKind::ArrayType), BracketLoc(BracketLoc),
          SizeExpr(Size), ConstraintExpr(Constraint), Base(Base),
          Qualifiers(Qualifiers) {}

        explicit
        ArrayTypeExpr(const SourceLocation BracketLoc,
                      Expr *const Size,
                      Expr *const Constraint,
                      Expr *const Base,
                      struct Qualifiers &&Qualifiers) noexcept
        : Expr(NodeKind::ArrayType), BracketLoc(BracketLoc),
          SizeExpr(Size), ConstraintExpr(Constraint), Base(Base),
          Qualifiers(std::move(Qualifiers)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == NodeKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getSizeExpr() const noexcept {
            return this->SizeExpr;
        }

        [[nodiscard]] constexpr auto getConstraintExpr() const noexcept {
            return this->ConstraintExpr;
        }

        [[nodiscard]] constexpr auto getBase() const noexcept {
            return this->Base;
        }

        [[nodiscard]] constexpr auto getBracketLoc() const noexcept {
            return this->BracketLoc;
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->getBracketLoc();
        }

        [[nodiscard]] inline auto &getQualifiers() const noexcept {
            return this->Qualifiers;
        }

        [[nodiscard]] inline auto &getQualifiersRef() noexcept {
            return this->Qualifiers;
        }

        constexpr auto setBase(Expr *const Base) noexcept -> decltype(*this) {
            this->Base = Base;
            return *this;
        }

        constexpr auto setSizeExpr(Expr *const SizeExpr) noexcept
            -> decltype(*this)
        {
            this->SizeExpr = SizeExpr;
            return *this;
        }

        constexpr auto setConstraintExpr(Expr *const ConstraintExpr) noexcept
            -> decltype(*this)
        {
            this->ConstraintExpr = ConstraintExpr;
            return *this;
        }

        constexpr auto setBracketLoc(const SourceLocation BracketLoc) noexcept
            -> decltype(*this)
        {
            this->BracketLoc = BracketLoc;
            return *this;
        }
    };
}
