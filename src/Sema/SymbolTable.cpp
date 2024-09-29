/*
 * Sema/SymbolTable.cpp
 */

#include "Sema/SymbolTable.h"

namespace Sema {
    SymbolTable::SymbolTable() noexcept : ScopeList() {
        // Add global scope
        addScope(new Scope());
    }

    auto SymbolTable::addScope(Scope *const Scope) noexcept -> bool {
        return ScopeList.emplace_back(Scope);
    }
}
