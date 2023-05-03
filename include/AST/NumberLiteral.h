/*
 * AST/NumberLiteral.h
 */

#pragma once
#include <cstdint>

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"
#include "Parse/Number.h"

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

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return (Stmt.getKind() == ObjKind);
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
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

        [[nodiscard]] llvm::Value *
        codegen(Backend::LLVM::Handler &Handler,
                llvm::IRBuilder<> &Builder,
                Backend::LLVM::ValueMap &ValueMap) noexcept;
    };
}