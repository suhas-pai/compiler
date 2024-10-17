/*
 * Sema/SymbolTable.h
 */

#pragma once
#include <vector>

#include "ADT/StringMap.h"
#include "Sema/Scope.h"

namespace Sema {
    struct SymbolTable {
    protected:
        std::vector<Scope *> ScopeList;
    public:
        explicit SymbolTable() noexcept;

        constexpr explicit
        SymbolTable(const std::vector<Scope *> &ScopeList) noexcept
        : ScopeList(ScopeList) {}

        constexpr explicit
        SymbolTable(std::vector<Scope *> &&ScopeList) noexcept
        : ScopeList(std::move(ScopeList)) {}

        [[nodiscard]] constexpr auto &getGlobalScope() const noexcept {
            return *ScopeList.front();
        }

        auto addScope(Scope *Scope) noexcept -> bool;
    };
}
