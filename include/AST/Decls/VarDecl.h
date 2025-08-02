/*
 * AST/Decls/VarDecl.h
 * © suhas pai
 */

#pragma once

#include "AST/Qualifiers.h"
#include "LvalueTypedDecl.h"

namespace AST {
    struct VarDecl : public LvalueTypedDecl {
    public:
        constexpr static auto ObjKind = NodeKind::VarDecl;
    protected:
        Qualifiers Quals;
    public:
        explicit
        VarDecl(const std::string_view Name,
                const SourceLocation NameLoc,
                const Qualifiers &Quals,
                Expr *const TypeExpr,
                Expr *const InitExpr = nullptr) noexcept
        : LvalueTypedDecl(ObjKind, Name, NameLoc, TypeExpr, InitExpr),
          Quals(Quals) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getInitExpr() const noexcept {
            return this->getRvalueExpr();
        }

        [[nodiscard]] auto &getQualifiers() const noexcept {
            return this->Quals;
        }

        [[nodiscard]] auto &getQualifiersRef() noexcept {
            return this->Quals;
        }

        constexpr auto setInitExpr(Expr *const InitExpr) noexcept
            -> decltype(*this)
        {
            this->setRvalueExpr(InitExpr);
            return *this;
        }
    };
}
