/*
 * AST/Decls/LvalueTypedDecl.h
 * © suhas pai
 */

#pragma once
#include "LvalueNamedDecl.h"

namespace AST {
    struct LvalueTypedDecl : public LvalueNamedDecl {
    protected:
        Expr *TypeExpr;

        constexpr explicit
        LvalueTypedDecl(const NodeKind ObjKind,
                        const std::string_view Name,
                        const SourceLocation NameLoc,
                        Expr *const TypeExpr,
                        Expr *const RvalueExpr) noexcept
        : LvalueNamedDecl(ObjKind, Name, NameLoc, RvalueExpr),
          TypeExpr(TypeExpr) {}

        constexpr explicit
        LvalueTypedDecl(const NodeKind ObjKind,
                        const std::string &&Name,
                        const SourceLocation NameLoc,
                        Expr *const TypeExpr,
                        Expr *const RvalueExpr) noexcept
        : LvalueNamedDecl(ObjKind, Name, NameLoc, RvalueExpr),
          TypeExpr(TypeExpr) {}
    public:
        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() >= NodeKind::LvalueTypedDeclBase &&
                   Stmt.getKind() <= NodeKind::LvalueTypedDeclLast;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getTypeExpr() const noexcept {
            return this->TypeExpr;
        }

        [[nodiscard]] constexpr auto hasInferredType() const noexcept {
            return this->getTypeExpr() == nullptr;
        }

        constexpr auto setTypeExpr(Expr *const TypeExpr) noexcept
            -> decltype(*this)
        {
            this->TypeExpr = TypeExpr;
            return *this;
        }
    };
}
