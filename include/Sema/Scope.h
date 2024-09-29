/*
 * Sema/Scope.h
 * © suhas pai
 */

#pragma once
#include <map>

#include "AST/Decl.h"
#include "ADT/StringMap.h"

namespace Sema {
    struct Scope {
    protected:
        Scope *Parent = nullptr;
        std::map<std::string_view, AST::Decl *> DeclMap;
    public:
        explicit Scope() noexcept = default;
        explicit Scope(Scope *const Parent) noexcept : Parent(Parent) {};

        auto addDecl(AST::Decl *Decl) noexcept -> Scope &;
        auto removeDecl(AST::Decl *Decl) noexcept -> Scope &;

        [[nodiscard]] constexpr auto &getDeclMap() const noexcept {
            return DeclMap;
        }

        [[nodiscard]]
        auto getDeclByName(std::string_view Name) const noexcept -> AST::Decl *;
    };
}
