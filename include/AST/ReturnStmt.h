/*
 * AST/ReturnStmt.h
 */

#pragma once

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct ReturnStmt : public Stmt {
    public:
        constexpr static auto ObjKind = NodeKind::ReturnStmt;
    protected:
        SourceLocation ReturnLoc;
        Expr *Value;
    public:
        constexpr explicit
        ReturnStmt(const SourceLocation ReturnLoc, Expr *const Value) noexcept
        : Stmt(ObjKind), ReturnLoc(ReturnLoc), Value(Value) {}

        [[nodiscard]] constexpr static auto none() noexcept {
            return ReturnStmt(SourceLocation::invalid(), /*Value=*/nullptr);
        }

        [[nodiscard]]
        constexpr static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getReturnLoc() const noexcept {
            return ReturnLoc;
        }

        [[nodiscard]] constexpr auto getValue() const noexcept {
            return Value;
        }

        constexpr auto setReturnLoc(const SourceLocation ReturnLoc) noexcept
            -> decltype(*this)
        {
            this->ReturnLoc = ReturnLoc;
            return *this;
        }

        constexpr auto setValue(Expr *const Value) noexcept -> decltype(*this) {
            this->Value = Value;
            return *this;
        }
    };
}
