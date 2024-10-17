/*
 * AST/Context.cpp
 * © suhas pai
 */

#include "AST/Context.h"

namespace AST {
    auto Context::addDecl(NamedDecl *const Decl) noexcept -> decltype(*this) {
        SymbolTable.getGlobalScope().addDecl(Decl);
        return *this;
    }

    auto
    Context::removeDecl(NamedDecl *const Decl) noexcept -> decltype(*this) {
        SymbolTable.getGlobalScope().removeDecl(Decl);
        return *this;
    }
}