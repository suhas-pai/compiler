/*
 * AST/Decls/LvalueNamedDecl.h
 * © suhas pai
 */

#pragma once
#include <string>

#include "AST/Expr.h"
#include "AST/NodeKind.h"

#include "Basic/SourceLocation.h"
#include "NamedDecl.h"

namespace AST {
    struct LvalueNamedDecl : public NamedDecl {
    public:
        constexpr static auto ObjKind = NodeKind::LvalueNamedDecl;
    protected:
        Expr *RvalueExpr;

        constexpr
        LvalueNamedDecl(const NodeKind ObjKind,
                        const std::string_view Name,
                        const SourceLocation NameLoc,
                        Expr *const RvalueExpr) noexcept
        : NamedDecl(ObjKind, Name, NameLoc), RvalueExpr(RvalueExpr) {}
    public:
        constexpr
        LvalueNamedDecl(const std::string_view Name,
                        const SourceLocation NameLoc,
                        Expr *const RvalueExpr) noexcept
        : NamedDecl(ObjKind, Name, NameLoc), RvalueExpr(RvalueExpr) {}

        constexpr
        LvalueNamedDecl(std::string &&Name,
                        const SourceLocation NameLoc,
                        Expr *const RvalueExpr) noexcept
        : NamedDecl(ObjKind, Name, NameLoc), RvalueExpr(RvalueExpr) {}

        [[nodiscard]] constexpr static inline auto IsOfKind(const Stmt &Stmt) {
            return Stmt.getKind() == ObjKind
                || Stmt.getKind() == NodeKind::VarDecl;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Stmt *const Stmt) {
            return IsOfKind(*Stmt);
        }

        [[nodiscard]] constexpr auto getRvalueExpr() const noexcept {
            return RvalueExpr;
        }

        constexpr auto setRvalueExpr(Expr *const RvalueExpr) noexcept
            -> decltype(*this)
        {
            this->RvalueExpr = RvalueExpr;
            return *this;
        }
    };
}
