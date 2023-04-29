/*
 * AST/VarDecl.h
 */

#pragma once
#include <string>
#include <string_view>

#include "AST/Stmt.h"
#include "Basic/SourceLocation.h"
#include "Lex/Token.h"

namespace AST {
    struct VarDecl : public Stmt {
    public:
        constexpr static auto ObjKind = ExprKind::VarDecl;
    protected:
        SourceLocation NameLoc;
        std::string Name;

        Expr *InitExpr;
    public:
        constexpr explicit
        VarDecl(const Lex::Token NameToken,
                const std::string_view Name,
                Expr *const InitExpr = nullptr) noexcept
        : Stmt(ObjKind), NameLoc(NameToken.Loc), Name(Name),
          InitExpr(InitExpr) {}

        [[nodiscard]] static inline auto IsOfKind(const Expr &Expr) noexcept {
            return (Expr.getKind() == ObjKind);
        }

        [[nodiscard]]
        static inline auto classof(const Expr *const Obj) noexcept {
            return IsOfKind(*Obj);
        }

        [[nodiscard]] constexpr auto getNameLoc() const noexcept {
            return NameLoc;
        }

        [[nodiscard]]
        constexpr auto getName() const noexcept -> std::string_view {
            return Name;
        }

        [[nodiscard]] constexpr auto getInitExpr() const noexcept {
            return InitExpr;
        }

        constexpr
        auto setName(const std::string_view Name) noexcept -> decltype(*this) {
            this->Name = Name;
            return *this;
        }

        constexpr
        auto setInitExpr(Expr *const InitExpr) noexcept -> decltype(*this) {
            this->InitExpr = InitExpr;
            return *this;
        }

        [[nodiscard]]
        llvm::Value *codegen(Backend::LLVM::Handler &Handler) noexcept override;
    };
}