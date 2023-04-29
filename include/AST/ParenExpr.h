/*
 * AST/ParenExpr.h
 */

#pragma once

#include "AST/Expr.h"
#include "Lex/Token.h"

namespace AST {
    struct ParenExpr : public Expr {
    public:
        constexpr static auto ObjKind = ExprKind::Paren;
    protected:
        SourceLocation Loc;
        SourceLocation End;

        Expr *ChildExpr;
    public:
        constexpr explicit
        ParenExpr(const Lex::Token Token,
                  Expr *const ChildExpr = nullptr) noexcept
        : Expr(ObjKind), Loc(Token.Loc), End(Token.End), ChildExpr(ChildExpr) {}

        [[nodiscard]] static inline auto IsOfKind(const Expr &Expr) noexcept {
            return (Expr.getKind() == ObjKind);
        }

        [[nodiscard]]
        static inline auto classof(const Expr *const Obj) noexcept {
            return IsOfKind(*Obj);
        }

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

        [[nodiscard]]
        llvm::Value *codegen(Backend::LLVM::Handler &Handler) noexcept override;
    };
}