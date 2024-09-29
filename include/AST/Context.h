/*
 * AST/Context.h
 */

#pragma once

#include "ADT/StringMap.h"
#include "Sema/SymbolTable.h"

#include "Interface/DiagnosticsEngine.h"

namespace AST {
    struct Decl;
    struct Stmt;
    struct CompoundStmt;
    struct FunctionDecl;
    struct VarDecl;

    struct Context {
    public:
        struct VisitOptions {
            bool AllowShadowingDecls : 1 = false;
        };
    protected:
        Sema::SymbolTable SymbolTable;

        void
        visitStmt(AST::Stmt *const Stmt,
                  Sema::SymbolTable &ParentSymbolTable,
                  Interface::DiagnosticsEngine &Diag,
                  VisitOptions Options) const noexcept;

        void
        visitCompountStmt(AST::CompoundStmt *const Stmt,
                          Sema::SymbolTable &ParentSymbolTable,
                          Interface::DiagnosticsEngine &Diag,
                          VisitOptions Options) const noexcept;

        void
        visitFunctionDecl(AST::FunctionDecl *FuncDecl,
                          Sema::SymbolTable &ParentSymbolTable,
                          Interface::DiagnosticsEngine &Diag,
                          VisitOptions Options) const noexcept;

        void
        visitVarDecl(AST::VarDecl *const VarDecl,
                     Sema::SymbolTable &ParentSymbolTable,
                     Interface::DiagnosticsEngine &Diag,
                     const VisitOptions Options) const noexcept;
    public:
        explicit Context() noexcept {}

        auto addDecl(Decl *Decl) noexcept -> decltype(*this);
        auto removeDecl(Decl *Decl) noexcept -> decltype(*this);

        [[nodiscard]] constexpr auto &getSymbolTable() const noexcept {
            return SymbolTable;
        }

        [[nodiscard]] constexpr auto &getSymbolTableRef() noexcept {
            return SymbolTable;
        }

        void
        visitDecls(Interface::DiagnosticsEngine &Diag,
                   VisitOptions Options) const noexcept;
    };
}