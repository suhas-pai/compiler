/*
 * AST/VarDecl.h
 */

#pragma once

#include <string>
#include "AST/Decls/ValueDecl.h"

#include "AST/Expr.h"
#include "AST/VarQualifiers.h"

namespace AST {
    struct VarDecl : public ValueDecl {
    public:
        constexpr static auto ObjKind = NodeKind::VarDecl;
    protected:
        Expr *InitExpr;
        VarQualifiers Qualifiers;

        bool IsGlobal : 1;
    public:
        constexpr explicit
        VarDecl(const std::string_view Name,
                const SourceLocation NameLoc,
                const VarQualifiers Qualifiers,
                TypeRef *const TypeRef,
                const bool IsGlobal,
                Expr *const InitExpr = nullptr) noexcept
        : ValueDecl(ObjKind, Name, NameLoc, Linkage::Private, TypeRef),
          InitExpr(InitExpr), Qualifiers(Qualifiers), IsGlobal(IsGlobal) {}

        constexpr explicit
        VarDecl(const std::string_view Name,
                const SourceLocation NameLoc,
                const VarQualifiers Qualifiers,
                Sema::Type *const Type,
                const bool IsGlobal,
                Expr *const InitExpr = nullptr) noexcept
        : ValueDecl(ObjKind, Name, NameLoc, Linkage::Private, Type),
          InitExpr(InitExpr),  Qualifiers(Qualifiers), IsGlobal(IsGlobal) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getInitExpr() const noexcept {
            return InitExpr;
        }

        [[nodiscard]] constexpr auto isGlobal() const noexcept {
            return IsGlobal;
        }

        [[nodiscard]] constexpr auto getQualifiers() const noexcept {
            return Qualifiers;
        }

        [[nodiscard]] constexpr auto &getQualifiersRef() noexcept {
            return Qualifiers;
        }

        constexpr auto setName(std::string &&Name) noexcept -> decltype(*this) {
            this->Name = std::move(Name);
            return *this;
        }

        constexpr
        auto setInitExpr(Expr *const InitExpr) noexcept -> decltype(*this) {
            this->InitExpr = InitExpr;
            return *this;
        }

        constexpr auto setIsGlobal(const bool IsGlobal) noexcept
            -> decltype(*this)
        {
            this->IsGlobal = IsGlobal;
            return *this;
        }
    };
}