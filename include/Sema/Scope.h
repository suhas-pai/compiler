/*
 * Sema/Scope.h
 * © suhas pai
 */

#pragma once
#include <map>

#include "AST/Decls/ValueDecl.h"
#include "ADT/StringMap.h"

namespace Sema {
    struct Scope {
    protected:
        Scope *Parent = nullptr;
        std::map<std::string_view, AST::ValueDecl *> DeclMap;
    public:
        explicit Scope() noexcept = default;
        explicit Scope(Scope *const Parent) noexcept : Parent(Parent) {};

        auto addDecl(AST::ValueDecl *Decl) noexcept -> Scope &;
        auto removeDecl(AST::ValueDecl *Decl) noexcept -> Scope &;

        [[nodiscard]] constexpr auto &getDeclMap() const noexcept {
            return DeclMap;
        }

        [[nodiscard]]
        auto getDeclByName(std::string_view Name) const noexcept
            -> AST::ValueDecl *;
    };
}
