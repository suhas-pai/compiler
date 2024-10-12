/*
 * AST/FieldExpr.h
 * © suhas pai
 */

#pragma once
#include <string>

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct FieldExpr : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::FieldExpr;
    protected:
        SourceLocation Loc;
        Expr *Base;

        std::string MemberName;
    public:
        constexpr explicit
        FieldExpr(const SourceLocation Loc,
                  Expr *const Base,
                  const std::string_view MemberName)
        : Expr(ObjKind), Loc(Loc), Base(Base), MemberName(MemberName) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Node) noexcept {
            return Node.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return Loc;
        }

        [[nodiscard]] constexpr auto getBase() const noexcept {
            return Base;
        }

        [[nodiscard]]
        constexpr auto getMemberName() const noexcept -> std::string_view {
            return MemberName;
        }

        constexpr auto setBase(Expr *const Base) noexcept -> decltype(*this) {
            this->Base = Base;
            return *this;
        }

        constexpr auto
        setMemberName(const std::string_view MemberName) noexcept
            -> decltype(*this)
        {
            this->MemberName = MemberName;
            return *this;
        }
    };
}