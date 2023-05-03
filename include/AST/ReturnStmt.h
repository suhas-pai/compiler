/*
 * AST/ReturnStmt.h
 */

#pragma once
#include "AST/Expr.h"
#include "Backend/LLVM/Handler.h"
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
        ReturnStmt(const SourceLocation ReturnLoc, Expr *const Value)
        : Stmt(ObjKind), ReturnLoc(ReturnLoc), Value(Value) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return (Stmt.getKind() == ObjKind);
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getReturnLoc() const noexcept {
            return ReturnLoc;
        }

        [[nodiscard]] constexpr auto getExpr() const noexcept {
            return Value;
        }

        constexpr auto setReturnLoc(const SourceLocation ReturnLoc) noexcept
            -> decltype(*this)
        {
            this->ReturnLoc = ReturnLoc;
            return *this;
        }

        constexpr auto setExpr(Expr *const Value) noexcept -> decltype(*this) {
            this->Value = Value;
            return *this;
        }

        [[nodiscard]] llvm::Value *
        codegen(Backend::LLVM::Handler &Handler,
                llvm::IRBuilder<> &Builder,
                Backend::LLVM::ValueMap &ValueMap) noexcept override;
    };
}
