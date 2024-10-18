/*
 * AST/Decls/FunctionDecl.h
 * © suhas pai
 */

#pragma once
#include <vector>

#include "AST/Decls/ParamVarDecl.h"

#include "AST/Expr.h"
#include "AST/Linkage.h"

#include "Basic/SourceLocation.h"

namespace AST {
    struct FunctionDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::FunctionDecl;
    protected:
        std::vector<ParamVarDecl *> ParamList;
        std::variant<Sema::Type *, TypeRef *> ReturnTypeOrRef;

        SourceLocation Loc;
        Linkage Linkage;

        Stmt *Body;
    public:
        constexpr explicit
        FunctionDecl(const SourceLocation Loc,
                     std::vector<ParamVarDecl *> &&ParamList,
                     TypeRef *const ReturnType,
                     Stmt *const Body,
                     const enum Linkage Linkage) noexcept
        : Expr(ObjKind), ParamList(std::move(ParamList)),
          ReturnTypeOrRef(ReturnType), Loc(Loc), Linkage(Linkage), Body(Body) {}

        constexpr explicit
        FunctionDecl(const SourceLocation Loc,
                     const std::vector<ParamVarDecl *> &ParamList,
                     TypeRef *const ReturnType,
                     Stmt *const Body,
                     const enum Linkage Linkage) noexcept
        : Expr(ObjKind), ParamList(ParamList), ReturnTypeOrRef(ReturnType),
          Loc(Loc), Linkage(Linkage), Body(Body) {}

        constexpr explicit
        FunctionDecl(const SourceLocation Loc,
                     const std::vector<ParamVarDecl *> &ParamList,
                     Sema::Type *const ReturnType,
                     Stmt *const Body,
                     const enum Linkage Linkage) noexcept
        : Expr(ObjKind), ParamList(ParamList), ReturnTypeOrRef(ReturnType),
          Loc(Loc), Linkage(Linkage), Body(Body) {}

        constexpr explicit
        FunctionDecl(const SourceLocation Loc,
                     std::vector<ParamVarDecl *> &&ParamList,
                     Sema::Type *const ReturnType,
                     Stmt *const Body,
                     const enum AST::Linkage Linkage) noexcept
        : Expr(ObjKind), ParamList(std::move(ParamList)),
          ReturnTypeOrRef(ReturnType), Loc(Loc), Linkage(Linkage), Body(Body) {}

        [[nodiscard]]
        constexpr static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getReturnType() const noexcept {
            return std::get<Sema::Type *>(ReturnTypeOrRef);
        }

        [[nodiscard]] constexpr auto getReturnTypeRef() const noexcept {
            return std::get<TypeRef *>(ReturnTypeOrRef);
        }

        [[nodiscard]] constexpr auto &getParamList() const noexcept {
            return ParamList;
        }

        [[nodiscard]] constexpr auto &getParamListRef() noexcept {
            return ParamList;
        }

        [[nodiscard]] constexpr auto getBody() const noexcept {
            return Body;
        }

        constexpr auto setBody(Stmt *const Body) noexcept -> decltype(*this) {
            this->Body = Body;
            return *this;
        }

        constexpr auto setReturnTypeRef(TypeRef *const ReturnType) noexcept
            -> decltype(*this)
        {
            this->ReturnTypeOrRef = ReturnType;
            return *this;
        }

        constexpr auto setReturnType(Sema::Type *const ReturnType) noexcept
            -> decltype(*this)
        {
            this->ReturnTypeOrRef = ReturnType;
            return *this;
        }
    };
}
