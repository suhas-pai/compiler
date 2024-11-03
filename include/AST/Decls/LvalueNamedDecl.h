/*
 * AST/Decls/LvalueNamedDecl.h
 * © suhas pai
 */

#pragma once
#include <string>

#include "AST/Expr.h"
#include "AST/NodeKind.h"

#include "Basic/SourceLocation.h"

namespace AST {
    struct LvalueNamedDecl : public Stmt {
    public:
        constexpr static auto ObjKind = NodeKind::LvalueNamedDecl;
    protected:
        std::string Name;
        SourceLocation NameLoc;

        Expr *RvalueExpr;

        constexpr
        LvalueNamedDecl(const NodeKind ObjKind,
                        const std::string_view Name,
                        const SourceLocation NameLoc,
                        Expr *const RvalueExpr) noexcept
        : Stmt(ObjKind), Name(Name), NameLoc(NameLoc), RvalueExpr(RvalueExpr) {}
    public:
        constexpr
        LvalueNamedDecl(const std::string_view Name,
                        const SourceLocation NameLoc,
                        Expr *const RvalueExpr) noexcept
        : Stmt(ObjKind), Name(Name), NameLoc(NameLoc), RvalueExpr(RvalueExpr) {}

        constexpr
        LvalueNamedDecl(std::string &&Name,
                        const SourceLocation NameLoc,
                        Expr *const RvalueExpr) noexcept
        : Stmt(ObjKind), Name(Name), NameLoc(NameLoc), RvalueExpr(RvalueExpr) {}

        [[nodiscard]] constexpr static auto IsOfKind(const Stmt &Stmt) {
            return Stmt.getKind() >= NodeKind::LvalueNamedDeclBase
                && Stmt.getKind() <= NodeKind::LvalueNamedDeclLast;
        }

        [[nodiscard]] constexpr static auto classof(const Stmt *const Stmt) {
            return IsOfKind(*Stmt);
        }

        [[nodiscard]]
        constexpr auto getName() const noexcept -> std::string_view {
            return Name;
        }

        [[nodiscard]] constexpr auto getNameLoc() const noexcept {
            return NameLoc;
        }

        [[nodiscard]] constexpr auto getRvalueExpr() const noexcept {
            return RvalueExpr;
        }

        constexpr auto setName(const std::string_view Name) noexcept
            -> decltype(*this)
        {
            this->Name = Name;
            return *this;
        }

        constexpr auto setNameLoc(const SourceLocation NameLoc) noexcept
            -> decltype(*this)
        {
            this->NameLoc = NameLoc;
            return *this;
        }

        constexpr auto setRvalueExpr(Expr *const RvalueExpr) noexcept
            -> decltype(*this)
        {
            this->RvalueExpr = RvalueExpr;
            return *this;
        }
    };
}
