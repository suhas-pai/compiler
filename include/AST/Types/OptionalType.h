/*
 * AST/Types/OptionalType.h
 * © suhas pai
 */

#pragma once

#include "AST/Expr.h"
#include "Source/SourceLocation.h"

namespace AST {
    class OptionalType : public Expr {
    protected:
        SourceLocation QuestionMarkLoc;
        Expr *Base;
    public:
        constexpr explicit
        OptionalType(const SourceLocation QuestionMarkLoc,
                     Expr *const Base) noexcept
        : Expr(NodeKind::OptionalType), QuestionMarkLoc(QuestionMarkLoc),
          Base(Base) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == NodeKind::OptionalType;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getQuestionMarkLoc() const noexcept {
            return this->QuestionMarkLoc;
        }

        [[nodiscard]] constexpr auto getBase() const noexcept {
            return this->Base;
        }

        constexpr auto setBase(Expr *const Base) noexcept -> decltype(*this) {
            this->Base = Base;
            return *this;
        }

        constexpr
        auto setQuestionMarkLoc(const SourceLocation QuestionMarkLoc) noexcept
            -> decltype(*this)
        {
            this->QuestionMarkLoc = QuestionMarkLoc;
            return *this;
        }
    };
}
