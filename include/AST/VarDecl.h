/*
 * AST/VarDecl.h
 */

#pragma once

#include <string>

#include "AST/TypeRef.h"
#include "AST/VarQualifiers.h"

#include "Lex/Token.h"
#include "Sema/Types/Type.h"

#include "Decl.h"

namespace AST {
    struct VarDecl : public Decl {
    public:
        constexpr static auto ObjKind = NodeKind::VarDecl;
    protected:
        std::string Name;
        Expr *InitExpr;

        VarQualifiers Qualifiers;
        std::variant<Sema::Type *, AST::TypeRef *> TypeOrTypeRef;

        bool IsGlobal : 1;
    public:
        constexpr explicit
        VarDecl(const Lex::Token NameToken,
                const std::string_view Name,
                const VarQualifiers Qualifiers,
                AST::TypeRef *const TypeRef,
                const bool IsGlobal,
                Expr *const InitExpr = nullptr) noexcept
        : Decl(ObjKind, NameToken.Loc, Linkage::Private), Name(Name),
          InitExpr(InitExpr), Qualifiers(Qualifiers), TypeOrTypeRef(TypeRef),
          IsGlobal(IsGlobal) {}

        constexpr explicit
        VarDecl(const Lex::Token NameToken,
                const std::string_view Name,
                const VarQualifiers Qualifiers,
                Sema::Type *const Type,
                const bool IsGlobal,
                Expr *const InitExpr = nullptr) noexcept
        : Decl(ObjKind, NameToken.Loc, Linkage::Private), Name(Name),
          InitExpr(InitExpr),  Qualifiers(Qualifiers), TypeOrTypeRef(Type),
          IsGlobal(IsGlobal) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]]
        constexpr std::string_view getName() const noexcept override {
            return Name;
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

        [[nodiscard]] constexpr auto getSemaType() {
            return std::get<Sema::Type *>(TypeOrTypeRef);
        }

        [[nodiscard]] constexpr auto getTypeRef() {
            return std::get<AST::TypeRef *>(TypeOrTypeRef);
        }

        [[nodiscard]] constexpr auto hasSemaType() {
            return std::holds_alternative<Sema::Type *>(TypeOrTypeRef);
        }

        constexpr
        auto setName(const std::string_view Name) noexcept -> decltype(*this) {
            this->Name = Name;
            return *this;
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

        constexpr auto setSemaType(Sema::Type *const Type) noexcept
            -> decltype(*this)
        {
            TypeOrTypeRef = Type;
            return *this;
        }

        constexpr auto setTypeRef(AST::TypeRef *const TypeRef) noexcept
            -> decltype(*this)
        {
            TypeOrTypeRef = TypeRef;
            return *this;
        }
    };
}