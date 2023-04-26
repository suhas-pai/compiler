
/*
 * AST/CharacterLiteral.h
 */

#pragma once

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"
#include "Lex/Token.h"

namespace AST {
    struct CharLiteral : public Expr {
    protected:
        SourceLocation Loc;
        char Value;
    public:
        constexpr explicit
        CharLiteral(const SourceLocation Loc, const char Value)
        : Expr(ExprKind::CharLiteral), Loc(Loc), Value(Value) {}

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return Loc;
        }

        [[nodiscard]] constexpr auto getValue() const noexcept {
            return Value;
        }

        constexpr auto setValue(const char Value) noexcept -> decltype(*this) {
            this->Value = Value;
            return *this;
        }

        [[nodiscard]]
        llvm::Value *codegen(Backend::LLVM::Handler &Handler) noexcept override;
    };
}