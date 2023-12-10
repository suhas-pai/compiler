/*
 * AST/VarDecl.h
 */

#pragma once

#include <string>

#include "AST/Decl.h"
#include "AST/Expr.h"
#include "Lex/Token.h"

namespace AST {
    struct VarDecl : public Decl {
    public:
        constexpr static auto ObjKind = NodeKind::VarDecl;
    protected:
        SourceLocation NameLoc;
        std::string Name;

        Expr *InitExpr;
        bool IsConstant : 1;
    public:
        constexpr explicit
        VarDecl(const Lex::Token NameToken,
                const std::string_view Name,
                bool IsConstant,
                Expr *const InitExpr = nullptr) noexcept
        : Decl(ObjKind), NameLoc(NameToken.Loc), Name(Name),
          InitExpr(InitExpr), IsConstant(IsConstant) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getNameLoc() const noexcept {
            return NameLoc;
        }

        [[nodiscard]]
        constexpr std::string_view getName() const noexcept override {
            return Name;
        }

        [[nodiscard]] constexpr auto getInitExpr() const noexcept {
            return InitExpr;
        }

        [[nodiscard]] constexpr auto isConstant() const noexcept {
            return IsConstant;
        }

        constexpr
        auto setName(const std::string_view Name) noexcept -> decltype(*this) {
            this->Name = Name;
            return *this;
        }

        constexpr auto
        setNameLoc(const SourceLocation NameLoc) noexcept -> decltype(*this) {
            this->NameLoc = NameLoc;
            return *this;
        }

        constexpr
        auto setInitExpr(Expr *const InitExpr) noexcept -> decltype(*this) {
            this->InitExpr = InitExpr;
            return *this;
        }

        constexpr auto setIsConstant(const bool IsConstant) noexcept
            -> decltype(*this)
        {
            this->IsConstant = IsConstant;
            return *this;
        }
    };
}