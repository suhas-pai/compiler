
/*
 * AST/CharacterLiteral.h
 */

#pragma once

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct CharLiteral : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::CharLiteral;
    protected:
        SourceLocation Loc;
        char Value;
    public:
        constexpr explicit
        CharLiteral(const SourceLocation Loc, const char Value) noexcept
        : Expr(ObjKind), Loc(Loc), Value(Value) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

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
    };
}