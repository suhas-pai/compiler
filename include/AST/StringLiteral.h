/*
 * AST/StringLiteral.h
 */

#pragma once

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"
#include "Lex/Token.h"

namespace AST {
    struct StringLiteral : Expr {
    protected:
        SourceLocation Loc;
        std::string Value;
    public:
        constexpr explicit
        StringLiteral(const SourceLocation Loc,
                      const std::string_view Value) noexcept
        : Expr(ExprKind::StringLiteral), Loc(Loc), Value(Value) {}

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return Loc;
        }

        [[nodiscard]] constexpr auto getValue() const noexcept {
            return Value;
        }

        constexpr auto
        setValue(const std::string_view Value) noexcept -> decltype(*this) {
            this->Value = Value;
            return *this;
        }

        [[nodiscard]]
        llvm::Value *codegen(Backend::LLVM::Handler &Handler) noexcept override;
    };
}