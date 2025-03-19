/*
 * AST/Types/FunctionType.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Decls/ParamVarDecl.h"
#include "AST/Expr.h"

namespace AST {
    struct FunctionTypeExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::FunctionType;
    protected:
        SourceLocation Loc;
        std::vector<ParamVarDecl *> ParamList;
        Expr *ReturnType;
    public:
        constexpr explicit
        FunctionTypeExpr(const SourceLocation Loc,
                         const std::span<ParamVarDecl *> ParamList,
                         Expr *const ReturnType) noexcept
        : Expr(ObjKind), Loc(Loc),
          ParamList(ParamList.begin(), ParamList.end()),
          ReturnType(ReturnType) {}

        constexpr explicit
        FunctionTypeExpr(const SourceLocation Loc,
                         std::vector<ParamVarDecl *> &&ParamList,
                         Expr *const ReturnType) noexcept
        : Expr(ObjKind), Loc(Loc), ParamList(std::move(ParamList)),
          ReturnType(ReturnType) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->Loc;
        }

        [[nodiscard]] constexpr auto getParamList() const noexcept {
            return std::span(this->ParamList);
        }

        [[nodiscard]] constexpr auto &getParamListRef() noexcept {
            return this->ParamList;
        }

        [[nodiscard]] constexpr auto getReturnType() const noexcept {
            return this->ReturnType;
        }

        constexpr auto setReturnTypeRef(Expr *const ReturnType) noexcept
            -> decltype(*this)
        {
            this->ReturnType = ReturnType;
            return *this;
        }
    };
}
