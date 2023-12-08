/*
 * AST/Context.h
 */

#pragma once
#include <map>

#include "ADT/StringMap.h"
#include "ADT/SymbolTable.h"
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
        std::map<std::string_view, AST::Decl *> DeclMap;
        ADT::SymbolTable SymbolTable;

        void
        visitStmt(AST::Stmt *const Stmt,
                  ADT::SymbolTable &ParentSymbolTable,
                  Interface::DiagnosticsEngine &Diag,
                  VisitOptions Options) const noexcept;

        void
        visitCompountStmt(AST::CompoundStmt *const Stmt,
                          ADT::SymbolTable &ParentSymbolTable,
                          Interface::DiagnosticsEngine &Diag,
                          VisitOptions Options) const noexcept;

        void
        visitFunctionDecl(AST::FunctionDecl *FuncDecl,
                          ADT::SymbolTable &ParentSymbolTable,
                          Interface::DiagnosticsEngine &Diag,
                          VisitOptions Options) const noexcept;

        void
        visitVarDecl(AST::VarDecl *const VarDecl,
                     ADT::SymbolTable &ParentSymbolTable,
                     Interface::DiagnosticsEngine &Diag,
                     const VisitOptions Options) const noexcept;
    public:
        explicit Context() noexcept {}

        auto addDecl(Decl *Decl) noexcept -> bool;
        auto removeDecl(Decl *Decl) noexcept -> void;

        [[nodiscard]] constexpr auto &getDeclMap() const noexcept {
            return DeclMap;
        }

        [[nodiscard]] constexpr auto &getDeclMapRef() noexcept {
            return DeclMap;
        }

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