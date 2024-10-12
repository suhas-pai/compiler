/*
 * Sema/Scope.cpp
 * © suhas pai
 */

#include "Sema/Scope.h"

namespace Sema {
    auto Scope::addDecl(AST::ValueDecl *const Decl) noexcept -> Scope & {
        DeclMap.insert({ Decl->getName(), Decl });
        return *this;
    }

    auto Scope::removeDecl(AST::ValueDecl *const Decl) noexcept -> Scope & {
        DeclMap.erase(Decl->getName());
        return *this;
    }

    auto Scope::getDeclByName(const std::string_view Name) const noexcept
        -> AST::ValueDecl *
    {
        if (!DeclMap.contains(Name)) {
            return nullptr;
        }

        return DeclMap.at(Name);
    }
}