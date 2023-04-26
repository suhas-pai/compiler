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
    protected:
        SourceLocation Loc;
        Parse::ParseNumberResult Number;
    public:
        constexpr explicit
        NumberLiteral(const SourceLocation Loc,
                      const Parse::ParseNumberResult Number) noexcept
        : Expr(ExprKind::NumberLiteral), Loc(Loc), Number(Number) {}

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