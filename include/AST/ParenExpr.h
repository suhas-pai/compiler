/*
 * AST/ParenExpr.h
 */

#pragma once

#include "AST/Expr.h"
#include "Lex/Token.h"

namespace AST {
    struct ParenExpr : public Expr {
    protected:
        SourceLocation Loc;
        SourceLocation End;

        Expr *ChildExpr;
    public:
        constexpr explicit
        ParenExpr(const Lex::Token Token,
                  Expr *const ChildExpr = nullptr) noexcept
        : Expr(ExprKind::Paren), Loc(Token.Loc), End(Token.End),
          ChildExpr(ChildExpr) {}

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return Loc;
        }

        [[nodiscard]] constexpr auto getEnd() const noexcept {
            return End;
        }

        [[nodiscard]] constexpr auto getChildExpr() const noexcept {
            return ChildExpr;
        }

        constexpr auto setChildExpr(Expr *const ChildExpr) noexcept
            -> decltype(*this)
        {
            this->ChildExpr = ChildExpr;
            return *this;
        }

        [[nodiscard]] llvm::Value *codegen() noexcept override;
    };
}