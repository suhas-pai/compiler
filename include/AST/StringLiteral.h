/*
 * AST/StringLiteral.h
 */

#pragma once

#include <string>

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct StringLiteral : Expr {
    public:
        constexpr static auto ObjKind = NodeKind::StringLiteral;
    protected:
        SourceLocation Loc;
        std::string Value;
    public:
        constexpr explicit
        StringLiteral(const SourceLocation Loc,
                      const std::string_view Value) noexcept
        : Expr(ObjKind), Loc(Loc), Value(Value) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
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
    };
}