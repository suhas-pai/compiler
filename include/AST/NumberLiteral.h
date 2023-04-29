/*
 * AST/NumberLiteral.h
 */

#pragma once

#include <cstdint>

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"
#include "Lex/Token.h"
#include "Parse/Number.h"

namespace AST {
    struct NumberLiteral : public Expr {
    public:
        constexpr static auto ObjKind = ExprKind::NumberLiteral;
    protected:
        SourceLocation Loc;
        Parse::ParseNumberResult Number;
    public:
        constexpr explicit
        NumberLiteral(const SourceLocation Loc,
                      const Parse::ParseNumberResult Number) noexcept
        : Expr(ObjKind), Loc(Loc), Number(Number) {}

        [[nodiscard]] static inline auto IsOfKind(const Expr &Expr) noexcept {
            return (Expr.getKind() == ObjKind);
        }

        [[nodiscard]]
        static inline auto classof(const Expr *const Obj) noexcept {
            return IsOfKind(*Obj);
        }

        [[nodiscard]] constexpr auto getNumber() const noexcept {
            return this->Number;
        }

        constexpr auto
        setNumber(const Parse::ParseNumberResult &Number) noexcept
            -> decltype(*this)
        {
            this->Number = Number;
            return *this;
        }

        [[nodiscard]]
        llvm::Value *codegen(Backend::LLVM::Handler &Handler) noexcept override;
    };
}