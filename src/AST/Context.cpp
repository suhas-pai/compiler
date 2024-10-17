/*
 * AST/Context.cpp
 * © suhas pai
 */

#include "AST/Context.h"

namespace AST {
    auto Context::addDecl(LvalueNamedDecl *const Decl) noexcept
        -> decltype(*this)
    {
        SymbolTable.getGlobalScope().addDecl(Decl);
        return *this;
    }

    auto
    Context::removeDecl(LvalueNamedDecl *const Decl) noexcept
        -> decltype(*this)
    {
        SymbolTable.getGlobalScope().removeDecl(Decl);
        return *this;
    }
}