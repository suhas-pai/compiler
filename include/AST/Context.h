/*
 * AST/Context.h
 */

#pragma once

#include "ADT/StringMap.h"
#include "Sema/SymbolTable.h"

#include "Interface/DiagnosticsEngine.h"

namespace AST {
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
        visitStmt(Stmt *Stmt,
                  Sema::SymbolTable &ParentSymbolTable,
                  Interface::DiagnosticsEngine &Diag,
                  VisitOptions Options) const noexcept;

        void
        visitCompoundStmt(CompoundStmt *Stmt,
                          Sema::SymbolTable &ParentSymbolTable,
                          Interface::DiagnosticsEngine &Diag,
                          VisitOptions Options) const noexcept;

        void
        visitFunctionDecl(FunctionDecl *FuncDecl,
                          Sema::SymbolTable &ParentSymbolTable,
                          Interface::DiagnosticsEngine &Diag,
                          VisitOptions Options) const noexcept;

        void
        visitVarDecl(VarDecl *VarDecl,
                     Sema::SymbolTable &ParentSymbolTable,
                     Interface::DiagnosticsEngine &Diag,
                     const VisitOptions Options) const noexcept;
    public:
        explicit Context() noexcept {}

        auto addDecl(LvalueNamedDecl *Decl) noexcept -> decltype(*this);
        auto removeDecl(LvalueNamedDecl *Decl) noexcept -> decltype(*this);

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