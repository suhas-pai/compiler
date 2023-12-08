/*
 * ADT/SymbolTable.cpp
 */

#include "AST/Decl.h"
#include "ADT/SymbolTable.h"

namespace ADT {
    auto SymbolTable::addDecl(AST::Decl *const Decl) noexcept -> bool {
        const auto Name = Decl->getName();
        if (getDeclMap().contains(Name)) {
            return false;
        }

        getDeclMapRef().insert(std::make_pair(Name, Decl));
        return true;
    }

    auto SymbolTable::getDecl(const std::string_view Name) const noexcept
        -> AST::Decl *
    {
        if (const auto Iter = getDeclMap().find(Name);
            Iter != getDeclMap().end())
        {
            return Iter->second;
        }

        return nullptr;
    }

    auto SymbolTable::hasDecl(const std::string_view Name) const noexcept
        -> bool
    {
        return getDecl(Name) != nullptr;
    }

    auto SymbolTable::removeDecl(const std::string_view Name) noexcept -> bool {
        if (const auto Iter = getDeclMap().find(Name);
            Iter != getDeclMap().end())
        {
            getDeclMapRef().erase(Iter);
            return true;
        }

        return false;
    }
}