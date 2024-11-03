/*
 * AST/Decls/FunctionDecl.h
 * © suhas pai
 */

#pragma once
#include <vector>

#include "AST/Linkage.h"
#include "ParamVarDecl.h"

namespace AST {
    struct FunctionDecl : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::FunctionDecl;

        struct Qualifiers {
        protected:
            bool IsInline : 1 = false;
            bool IsComptime : 1 = false;
            bool IsNoReturn : 1 = false;
            bool IsNoInline : 1 = false;
        public:
            constexpr explicit Qualifiers() noexcept = default;

            [[nodiscard]] constexpr auto isInline() const noexcept {
                return IsInline;
            }

            [[nodiscard]] constexpr auto isComptime() const noexcept {
                return IsComptime;
            }

            [[nodiscard]] constexpr auto isNoReturn() const noexcept {
                return IsNoReturn;
            }

            [[nodiscard]] constexpr auto isNoInline() const noexcept {
                return IsNoInline;
            }

            constexpr auto setIsInline(const bool IsInline) noexcept
                -> decltype(*this)
            {
                this->IsInline = IsInline;
                return *this;
            }

            constexpr auto setIsComptime(const bool IsComptime) noexcept
                -> decltype(*this)
            {
                this->IsComptime = IsComptime;
                return *this;
            }

            constexpr auto setIsNoReturn(const bool IsNoReturn) noexcept
                -> decltype(*this)
            {
                this->IsNoReturn = IsNoReturn;
                return *this;
            }

            constexpr auto setIsNoInline(const bool IsNoInline) noexcept
                -> decltype(*this)
            {
                this->IsNoInline = IsNoInline;
                return *this;
            }
        };
    protected:
        std::vector<ParamVarDecl *> ParamList;
        std::variant<Sema::Type *, TypeRef *> ReturnTypeOrRef;

        SourceLocation Loc;
        Linkage Linkage;
        Qualifiers Quals;

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
                     const enum Linkage Linkage) noexcept
        : Expr(ObjKind), ParamList(std::move(ParamList)),
          ReturnTypeOrRef(ReturnType), Loc(Loc), Linkage(Linkage), Body(Body) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
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

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return Loc;
        }

        [[nodiscard]] constexpr auto getLinkage() const noexcept {
            return Linkage;
        }

        [[nodiscard]] constexpr auto getQualifiers() const noexcept {
            return Quals;
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
