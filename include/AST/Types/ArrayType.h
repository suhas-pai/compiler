/*
 * AST/Types/ArrayType.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Expr.h"
#include "Source/SourceLocation.h"
#include "Sema/Types/PointerQualifiers.h"

namespace AST {
    struct ArrayTypeExpr : public Expr {
    public:
        constexpr static auto NodeKind = NodeKind::ArrayType;
    protected:
        std::vector<Stmt *> DetailList;

        SourceLocation BracketLoc;
        Expr *Base;

        Sema::PointerBaseTypeQualifiers Qualifiers;
    public:
        constexpr explicit
        ArrayTypeExpr(const SourceLocation BracketLoc,
                  const std::span<Stmt *> DetailList,
                  Expr *const Base,
                  const Sema::PointerBaseTypeQualifiers Qualifiers) noexcept
        : Expr(NodeKind::ArrayType),
          DetailList(std::vector(DetailList.begin(), DetailList.end())),
          BracketLoc(BracketLoc), Base(Base), Qualifiers(Qualifiers) {}

        constexpr explicit
        ArrayTypeExpr(const SourceLocation BracketLoc,
                  std::vector<Stmt *> &&DetailList,
                  Expr *const Base,
                  const Sema::PointerBaseTypeQualifiers Qualifiers) noexcept
        : Expr(NodeKind::ArrayType), DetailList(std::move(DetailList)),
          BracketLoc(BracketLoc), Base(Base), Qualifiers(Qualifiers) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == NodeKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getDetailList() const noexcept {
            return std::span(this->DetailList);
        }

        [[nodiscard]] constexpr auto &getDetailListRef() noexcept {
            return this->DetailList;
        }

        [[nodiscard]] constexpr auto getBracketLoc() const noexcept {
            return this->BracketLoc;
        }

        [[nodiscard]] constexpr auto getBase() const noexcept {
            return this->Base;
        }

        [[nodiscard]] constexpr auto getQualifiers() const noexcept {
            return this->Qualifiers;
        }

        [[nodiscard]] constexpr auto &getQualifiersRef() noexcept {
            return this->Qualifiers;
        }

        constexpr auto setBase(Expr *const Base) noexcept -> decltype(*this) {
            this->Base = Base;
            return *this;
        }

        constexpr auto setBracketLoc(const SourceLocation BracketLoc) noexcept
            -> decltype(*this)
        {
            this->BracketLoc = BracketLoc;
            return *this;
        }
    };
}
