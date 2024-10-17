/*
 * Sema/Scope.h
 * © suhas pai
 */

#pragma once
#include <map>

#include "AST/Decls/NamedDecl.h"
#include "ADT/StringMap.h"

namespace Sema {
    struct Scope {
    protected:
        Scope *Parent = nullptr;
        std::map<std::string_view, AST::NamedDecl *> DeclMap;
    public:
        explicit Scope() noexcept = default;
        explicit Scope(Scope *const Parent) noexcept : Parent(Parent) {};

        auto addDecl(AST::NamedDecl *Decl) noexcept -> Scope &;
        auto removeDecl(AST::NamedDecl *Decl) noexcept -> Scope &;

        [[nodiscard]] constexpr auto &getDeclMap() const noexcept {
            return DeclMap;
        }

        [[nodiscard]]
        auto getDeclByName(std::string_view Name) const noexcept
            -> AST::NamedDecl *;
    };
}
