/*
 * AST/StringLiteral.h
 */

#pragma once

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct StringLiteral : Expr {
    public:
        constexpr static auto ObjKind = ExprKind::StringLiteral;
    protected:
        SourceLocation Loc;
        std::string Value;
    public:
        constexpr explicit
        StringLiteral(const SourceLocation Loc,
                      const std::string_view Value) noexcept
        : Expr(ObjKind), Loc(Loc), Value(Value) {}

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