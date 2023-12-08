/*
 * include/AST/Decl.h
 */

#pragma once
#include "AST/Stmt.h"

namespace AST {
    struct Decl : public Stmt {
    public:
        enum class Linkage {
            Private,
            External,
        };
    protected:
        constexpr explicit Decl(const NodeKind Kind) noexcept : Stmt(Kind) {}
        Linkage Link = Linkage::Private;
    public:
        [[nodiscard]]
        virtual auto getName() const noexcept -> std::string_view = 0;

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return Node->getKind() >= NodeKind::DeclBase &&
                   Node->getKind() <= NodeKind::DeclLast;
        }

        [[nodiscard]] constexpr auto getLinkage() const noexcept -> Linkage {
            return Link;
        }

        constexpr
        auto setLinkage(const Linkage Link) noexcept -> decltype(*this) {
            this->Link = Link;
            return *this;
        }
    };
}