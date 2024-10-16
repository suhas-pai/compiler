/*
 * Sema/Scope.cpp
 * © suhas pai
 */

#include "Sema/Scope.h"

namespace Sema {
    auto Scope::addDecl(AST::LvalueNamedDecl *const Decl) noexcept
        -> decltype(*this)
    {
        DeclMap.insert({ Decl->getName(), Decl });
        return *this;
    }

    auto Scope::removeDecl(AST::LvalueNamedDecl *const Decl) noexcept
        -> decltype(*this)
    {
        DeclMap.erase(Decl->getName());
        return *this;
    }

    auto Scope::getDeclByName(const std::string_view Name) const noexcept
        -> AST::LvalueNamedDecl *
    {
        if (!DeclMap.contains(Name)) {
            return nullptr;
        }

        return DeclMap.at(Name);
    }
}
