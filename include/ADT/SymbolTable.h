/*
 * ADT/SymbolTable.h
 */

#pragma once

#include <map>
#include "StringMap.h"

namespace AST {
    struct Decl;
}

namespace ADT {
    struct SymbolTable {
    protected:
        SymbolTable *Parent;
        std::map<std::string_view, AST::Decl *> DeclMap;
    public:
        explicit SymbolTable(SymbolTable *const Parent = nullptr) noexcept
        : Parent(Parent) {}

        [[nodiscard]] constexpr auto getParent() const noexcept {
            return Parent;
        }

        [[nodiscard]] constexpr auto &getDeclMap() const noexcept {
            return DeclMap;
        }

        [[nodiscard]] constexpr auto &getDeclMapRef() noexcept {
            return DeclMap;
        }

        constexpr auto setParent(SymbolTable *const Parent) noexcept
            -> decltype(*this)
        {
            this->Parent = Parent;
            return *this;
        }

        auto addDecl(AST::Decl *Decl) noexcept -> bool;

        [[nodiscard]] auto hasDecl(const std::string_view Name) const noexcept
            -> bool;

        [[nodiscard]] auto getDecl(const std::string_view Name) const noexcept
            -> AST::Decl *;

        auto removeDecl(const std::string_view Name) noexcept -> bool;
    };
}