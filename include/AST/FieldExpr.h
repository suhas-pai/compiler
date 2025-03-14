/*
 * AST/FieldExpr.h
 * © suhas pai
 */

#pragma once

#include <string>
#include "Expr.h"

namespace AST {
    struct FieldExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::FieldExpr;
    protected:
        SourceLocation Loc;

        bool IsArrow : 1 = false;
        Expr *Base;

        std::string MemberName;
    public:
        constexpr explicit
        FieldExpr(const SourceLocation Loc,
                  Expr *const Base,
                  const bool IsArrow,
                  const std::string_view MemberName)
        : Expr(ObjKind), Loc(Loc), IsArrow(IsArrow), Base(Base),
          MemberName(MemberName) {}

        constexpr explicit
        FieldExpr(const SourceLocation Loc,
                  Expr *const Base,
                  const bool IsArrow,
                  std::string &&MemberName)
        : Expr(ObjKind), Loc(Loc), IsArrow(IsArrow), Base(Base),
          MemberName(std::move(MemberName)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] SourceLocation getLoc() const noexcept override {
            return this->Loc;
        }

        [[nodiscard]] constexpr auto getBase() const noexcept {
            return this->Base;
        }

        [[nodiscard]] constexpr auto isArrow() const noexcept {
            return this->IsArrow;
        }

        [[nodiscard]]
        constexpr auto getMemberName() const noexcept -> std::string_view {
            return this->MemberName;
        }

        constexpr auto setBase(Expr *const Base) noexcept -> decltype(*this) {
            this->Base = Base;
            return *this;
        }

        constexpr auto setIsArrow(const bool IsArrow) noexcept
            -> decltype(*this)
        {
            this->IsArrow = IsArrow;
            return *this;
        }

        constexpr auto setMemberName(const std::string_view MemberName) noexcept
            -> decltype(*this)
        {
            this->MemberName = MemberName;
            return *this;
        }

        constexpr auto setMemberName(std::string &&MemberName) noexcept
            -> decltype(*this)
        {
            this->MemberName = std::move(MemberName);
            return *this;
        }
    };
}