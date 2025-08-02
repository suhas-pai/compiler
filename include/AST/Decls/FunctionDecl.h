/*
 * AST/Decls/FunctionDecl.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Expr.h"
#include "AST/Qualifiers.h"

namespace AST {
    struct FunctionDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::FunctionDecl;
    protected:
        std::vector<Stmt *> ParamList;

        SourceLocation Loc;
        Qualifiers Quals;

        Expr *ReturnTypeExpr;
        Stmt *Body;

        explicit
        FunctionDecl(const NodeKind ObjKind,
                     const SourceLocation Loc,
                     const Qualifiers &Quals,
                     std::vector<Stmt *> &&ParamList,
                     Expr *const ReturnTypeExpr,
                     Stmt *const Body) noexcept
        : Expr(ObjKind), ParamList(std::move(ParamList)), Loc(Loc),
          Quals(Quals), ReturnTypeExpr(ReturnTypeExpr), Body(Body) {}

        explicit
        FunctionDecl(const NodeKind ObjKind,
                     const SourceLocation Loc,
                     Qualifiers &&Quals,
                     std::vector<Stmt *> &&ParamList,
                     Expr *const ReturnTypeExpr,
                     Stmt *const Body) noexcept
        : Expr(ObjKind), ParamList(std::move(ParamList)), Loc(Loc),
          Quals(std::move(Quals)), ReturnTypeExpr(ReturnTypeExpr), Body(Body) {}

        explicit
        FunctionDecl(const NodeKind ObjKind,
                     const SourceLocation Loc,
                     const Qualifiers &Quals,
                     const std::span<Stmt *> ParamList,
                     Expr *const ReturnTypeExpr,
                     Stmt *const Body) noexcept
        : Expr(ObjKind), ParamList(ParamList.begin(), ParamList.end()),
          Loc(Loc), Quals(Quals), ReturnTypeExpr(ReturnTypeExpr), Body(Body) {}

        explicit
        FunctionDecl(const NodeKind ObjKind,
                     const SourceLocation Loc,
                     Qualifiers &&Quals,
                     const std::span<Stmt *> ParamDeclList,
                     Expr *const ReturnTypeExpr,
                     Stmt *const Body) noexcept
        : Expr(ObjKind),
          ParamList(ParamDeclList.begin(), ParamDeclList.end()), Loc(Loc),
          Quals(std::move(Quals)), ReturnTypeExpr(ReturnTypeExpr), Body(Body) {}
    public:
        explicit
        FunctionDecl(const SourceLocation Loc,
                     Qualifiers &&Quals,
                     std::vector<Stmt *> &&ParamList,
                     Expr *const ReturnTypeExpr,
                     Stmt *const Body) noexcept
        : FunctionDecl(ObjKind, Loc, Quals, ParamList, ReturnTypeExpr, Body) {}

        explicit
        FunctionDecl(const SourceLocation Loc,
                     Qualifiers &&Quals,
                     const std::span<Stmt *> ParamList,
                     Expr *const ReturnTypeExpr,
                     Stmt *const Body) noexcept
        : FunctionDecl(ObjKind, Loc, Quals, ParamList, ReturnTypeExpr, Body) {}

        explicit
        FunctionDecl(const SourceLocation Loc,
                     const Qualifiers &Quals,
                     std::vector<Stmt *> &&ParamList,
                     Expr *const ReturnTypeExpr,
                     Stmt *const Body) noexcept
        : FunctionDecl(ObjKind, Loc, Quals, ParamList, ReturnTypeExpr, Body) {}

        explicit
        FunctionDecl(const SourceLocation Loc,
                     const Qualifiers &Quals,
                     const std::span<Stmt *> ParamList,
                     Expr *const ReturnTypeExpr,
                     Stmt *const Body) noexcept
        : FunctionDecl(ObjKind, Loc, Quals, ParamList, ReturnTypeExpr, Body) {}

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
            return std::span(this->ParamList);
        }

        [[nodiscard]] constexpr auto &getParamListRef() noexcept {
            return this->ParamList;
        }

        [[nodiscard]] constexpr auto getBody() const noexcept {
            return this->Body;
        }

        [[nodiscard]]
        constexpr SourceLocation getLoc() const noexcept override {
            return this->Loc;
        }

        [[nodiscard]] auto &getQualifiers() const noexcept {
            return this->Quals;
        }

        [[nodiscard]] auto &getQualifiersRef() noexcept {
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
