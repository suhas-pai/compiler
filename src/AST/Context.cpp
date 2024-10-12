/*
 * AST/Context.cpp
 * © suhas pai
 */

#include "AST/Context.h"

namespace AST {
    auto Context::addDecl(ValueDecl *const Decl) noexcept -> decltype(*this) {
        SymbolTable.getGlobalScope().addDecl(Decl);
        return *this;
    }

    auto
    Context::removeDecl(ValueDecl *const Decl) noexcept -> decltype(*this) {
        SymbolTable.getGlobalScope().removeDecl(Decl);
        return *this;
    }
}