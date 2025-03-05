/*
 * AST/Decls/ArrowFunctionDecl.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Decls/ParamVarDecl.h"
#include "AST/Qualifiers.h"

namespace AST {
    struct ArrowFunctionDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::ArrowFunctionDecl;
    protected:
        std::vector<ParamVarDecl *> ParamDeclList;

        SourceLocation Loc;
        Qualifiers Quals;

        Expr *BodyExpr;
    public:
        explicit
        ArrowFunctionDecl(const SourceLocation Loc,
                          std::vector<ParamVarDecl *> &&ParamList,
                          Expr *const Body) noexcept
        : Expr(ObjKind), ParamDeclList(std::move(ParamList)),
          Loc(Loc), BodyExpr(Body) {}

        explicit
        ArrowFunctionDecl(const SourceLocation Loc,
                          const std::span<ParamVarDecl *> ParamDeclList,
                          Expr *const Body) noexcept
        : Expr(ObjKind),
          ParamDeclList(
            std::vector(ParamDeclList.begin(), ParamDeclList.end())), Loc(Loc),
          BodyExpr(Body) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getParamList() const noexcept {
            return std::span(this->ParamDeclList);
        }

        [[nodiscard]] constexpr auto &getParamListRef() noexcept {
            return this->ParamDeclList;
        }

        [[nodiscard]] constexpr auto getBodyExpr() const noexcept {
            return this->BodyExpr;
        }

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return this->Loc;
        }

        [[nodiscard]] inline auto &getQualifiers() const noexcept {
            return this->Quals;
        }

        [[nodiscard]] inline auto &getQualifiersRef() noexcept {
            return this->Quals;
        }

        constexpr auto setBody(Expr *const Body) noexcept -> decltype(*this) {
            this->BodyExpr = Body;
            return *this;
        }
    };
}
