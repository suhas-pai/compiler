/*
 * AST/Context.h
 */

#pragma once

#include "ADT/UnorderedStringMap.h"
#include "Decl.h"
#include "Interface/DiagnosticsEngine.h"

namespace AST {
    struct Context {
    protected:
        ADT::UnorderedStringMap<AST::Decl *> DeclList;
        Interface::DiagnosticsEngine &Diag;
    public:
        explicit Context(Interface::DiagnosticsEngine &Diag) noexcept
        : Diag(Diag) {}

        auto addDecl(AST::Decl *const Decl) noexcept -> bool;

        [[nodiscard]] constexpr auto &getDeclMap() const noexcept {
            return DeclList;
        }

        [[nodiscard]] constexpr auto &getDeclMapRef() noexcept {
            return DeclList;
        }

        [[nodiscard]] constexpr auto &getDiag() const noexcept {
            return Diag;
        }
    };
}