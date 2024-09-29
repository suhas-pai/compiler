/*
 * include/AST/Decl.h
 */

#pragma once

#include <string_view>
#include "AST/Stmt.h"

#include "Basic/SourceLocation.h"

namespace AST {
    struct Decl : public Stmt {
    public:
        enum class Linkage {
            Private,
            Exported,
            External,
        };
    protected:
        SourceLocation Loc;
        Linkage Link = Linkage::Private;

        constexpr explicit
        Decl(const NodeKind Kind,
             const SourceLocation Loc,
             const Linkage Linkage) noexcept
        : Stmt(Kind), Loc(Loc), Link(Linkage) {}
    public:
        [[nodiscard]]
        virtual auto getName() const noexcept -> std::string_view = 0;

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return Loc;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return Node->getKind() >= NodeKind::DeclBase
                && Node->getKind() <= NodeKind::DeclLast;
        }

        [[nodiscard]] constexpr auto getLinkage() const noexcept {
            return Link;
        }

        constexpr
        auto setLinkage(const Linkage Link) noexcept -> decltype(*this) {
            this->Link = Link;
            return *this;
        }
    };
}