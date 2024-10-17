/*
 * AST/Decls/VarDecl.h
 * © suhas pai
 */

#pragma once

#include "AST/Linkage.h"
#include "AST/VarQualifiers.h"

#include "LvalueTypedDecl.h"

namespace AST {
    struct VarDecl : public LvalueTypedDecl {
    public:
        constexpr static auto ObjKind = NodeKind::VarDecl;
    protected:
        Linkage Linkage;
        VarQualifiers Qualifiers;
    public:
        constexpr explicit
        VarDecl(const std::string_view Name,
                const SourceLocation NameLoc,
                const VarQualifiers Qualifiers,
                TypeRef *const TypeRef,
                Expr *const InitExpr = nullptr) noexcept
        : LvalueTypedDecl(ObjKind, Name, NameLoc, InitExpr, TypeRef),
          Linkage(Linkage::Private), Qualifiers(Qualifiers) {}

        constexpr explicit
        VarDecl(const std::string_view Name,
                const SourceLocation NameLoc,
                const VarQualifiers Qualifiers,
                Sema::Type *const Type,
                Expr *const InitExpr = nullptr) noexcept
        : LvalueTypedDecl(ObjKind, Name, NameLoc, InitExpr, Type),
          Linkage(Linkage::Private), Qualifiers(Qualifiers) {}

        [[nodiscard]]
        constexpr static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getInitExpr() const noexcept {
            return getRvalueExpr();
        }

        [[nodiscard]] constexpr auto getQualifiers() const noexcept {
            return Qualifiers;
        }

        [[nodiscard]] constexpr auto &getQualifiersRef() noexcept {
            return Qualifiers;
        }

        constexpr auto setInitExpr(Expr *const InitExpr) noexcept
            -> decltype(*this)
        {
            setRvalueExpr(InitExpr);
            return *this;
        }
    };
}
