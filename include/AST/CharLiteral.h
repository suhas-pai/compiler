
/*
 * AST/CharacterLiteral.h
 */

#pragma once
#include "Expr.h"

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

        [[nodiscard]] SourceLocation getLoc() const noexcept override {
            return this->Loc;
        }

        [[nodiscard]] constexpr auto getValue() const noexcept {
            return this->Value;
        }

        constexpr auto setValue(const char Value) noexcept -> decltype(*this) {
            this->Value = Value;
            return *this;
        }
    };
}