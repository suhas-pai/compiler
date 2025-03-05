/*
 * AST/NumberLiteral.h
 */

#pragma once

#include "Parse/ParseNumber.h"
#include "Source/SourceLocation.h"

#include "Expr.h"

namespace AST {
    struct NumberLiteral : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::NumberLiteral;
    protected:
        SourceLocation Loc;
        Parse::ParseNumberResult Number;
    public:
        constexpr explicit
        NumberLiteral(const SourceLocation Loc,
                      const Parse::ParseNumberResult Number) noexcept
        : Expr(ObjKind), Loc(Loc), Number(Number) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
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
    };
}