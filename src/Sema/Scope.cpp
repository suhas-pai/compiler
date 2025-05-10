/*
 * Sema/Scope.cpp
 * © suhas pai
 */

#include "Sema/Scope.h"

namespace Sema {
    auto Scope::addDecl(AST::LvalueNamedDecl *const Decl) noexcept
        -> decltype(*this)
    {
        this->DeclMap.insert({ Decl->getName(), Decl });
        return *this;
    }

    auto Scope::removeDecl(AST::LvalueNamedDecl *const Decl) noexcept
        -> decltype(*this)
    {
        this->DeclMap.erase(Decl->getName());
        return *this;
    }

    auto Scope::getDeclByName(const std::string_view Name) const noexcept
        -> AST::LvalueNamedDecl *
    {
        if (const auto Found = this->DeclMap.find(Name);
            Found != this->DeclMap.end())
        {
            return Found->second;
        }

        return nullptr;
    }
}
