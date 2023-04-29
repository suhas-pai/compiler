/*
 * AST/VariableRef.h
 */

#pragma once

#include "Backend/LLVM/Handler.h"
#include "Basic/SourceLocation.h"
#include "Expr.h"
#include "llvm/IR/Value.h"

namespace AST {
    struct VariableRef : public Expr {
    public:
        constexpr static auto ObjKind = ExprKind::VariableRef;
    protected:
        SourceLocation NameLoc;
        std::string Name;
    public:
        constexpr explicit
        VariableRef(const SourceLocation NameLoc,
                    const std::string_view Name) noexcept
        : Expr(ObjKind), NameLoc(NameLoc), Name(Name) {}

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

        constexpr auto setName(const std::string_view Name) noexcept
            -> decltype(*this)
        {
            this->Name = Name;
            return *this;
        }

        constexpr auto
        setNameLoc(const SourceLocation NameLoc) noexcept -> decltype(*this) {
            this->NameLoc = NameLoc;
            return *this;
        }

        [[nodiscard]]
        llvm::Value *codegen(Backend::LLVM::Handler &Handler) noexcept override;
    };
}