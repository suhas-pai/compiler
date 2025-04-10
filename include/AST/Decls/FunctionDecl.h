/*
 * AST/Decls/FunctionDecl.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Qualifiers.h"
#include "ParamVarDecl.h"

namespace AST {
    struct FunctionDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::FunctionDecl;
    protected:
        std::vector<ParamVarDecl *> ParamDeclList;

        SourceLocation Loc;
        Qualifiers Quals;

        Expr *ReturnTypeExpr;
        Stmt *Body;

        explicit
        FunctionDecl(const NodeKind ObjKind,
                     const SourceLocation Loc,
                     std::vector<ParamVarDecl *> &&ParamList,
                     Expr *const ReturnTypeExpr,
                     Stmt *const Body) noexcept
        : Expr(ObjKind), ParamDeclList(std::move(ParamList)),
          Loc(Loc), ReturnTypeExpr(ReturnTypeExpr), Body(Body) {}

        explicit
        FunctionDecl(const NodeKind ObjKind,
                     const SourceLocation Loc,
                     const std::span<ParamVarDecl *> ParamDeclList,
                     Expr *const ReturnTypeExpr,
                     Stmt *const Body) noexcept
        : Expr(ObjKind),
          ParamDeclList(ParamDeclList.begin(), ParamDeclList.end()), Loc(Loc),
          ReturnTypeExpr(ReturnTypeExpr), Body(Body) {}
    public:
        explicit
        FunctionDecl(const SourceLocation Loc,
                     std::vector<ParamVarDecl *> &&ParamList,
                     Expr *const ReturnTypeExpr,
                     Stmt *const Body) noexcept
        : Expr(ObjKind), ParamDeclList(std::move(ParamList)),
          Loc(Loc), ReturnTypeExpr(ReturnTypeExpr), Body(Body) {}

        explicit
        FunctionDecl(const SourceLocation Loc,
                     const std::span<ParamVarDecl *> ParamDeclList,
                     Expr *const ReturnTypeExpr,
                     Stmt *const Body) noexcept
        : Expr(ObjKind),
          ParamDeclList(ParamDeclList.begin(), ParamDeclList.end()), Loc(Loc),
          ReturnTypeExpr(ReturnTypeExpr), Body(Body) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getReturnTypeExpr() const noexcept {
            return this->ReturnTypeExpr;
        }

        [[nodiscard]] constexpr auto getParamList() const noexcept {
            return std::span(this->ParamDeclList);
        }

        [[nodiscard]] constexpr auto &getParamListRef() noexcept {
            return this->ParamDeclList;
        }

        [[nodiscard]] constexpr auto getBody() const noexcept {
            return this->Body;
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->Loc;
        }

        [[nodiscard]] constexpr auto &getQualifiers() const noexcept {
            return this->Quals;
        }

        [[nodiscard]] constexpr auto &getQualifiersRef() noexcept {
            return this->Quals;
        }

        constexpr auto setBody(Stmt &Body) noexcept -> decltype(*this) {
            this->Body = &Body;
            return *this;
        }

        constexpr auto setReturnTypeExpr(Expr &ReturnTypeExpr) noexcept
            -> decltype(*this)
        {
            this->ReturnTypeExpr = &ReturnTypeExpr;
            return *this;
        }
    };
}
