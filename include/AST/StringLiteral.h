/*
 * AST/StringLiteral.h
 */

#pragma once

#include <string>
#include "Expr.h"

namespace AST {
    struct StringLiteral : public Expr {
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

        constexpr explicit
        StringLiteral(const SourceLocation Loc, std::string &&Value) noexcept
        : Expr(ObjKind), Loc(Loc), Value(std::move(Value)) {}

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

        [[nodiscard]]
        constexpr auto value() const noexcept -> std::string_view {
            return this->Value;
        }

        constexpr auto setValue(const std::string_view Value) noexcept
            -> decltype(*this)
        {
            this->Value = Value;
            return *this;
        }

        constexpr auto setValue(std::string &&Value) noexcept -> decltype(*this)
        {
            this->Value = std::move(Value);
            return *this;
        }
    };
}