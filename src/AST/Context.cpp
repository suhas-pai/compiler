/*
 * AST/Context.cpp
 */

#include "AST/Context.h"
#include "AST/Decl.h"
#include "AST/CompoundStmt.h"
#include "AST/FunctionDecl.h"
#include "AST/ReturnStmt.h"
#include "AST/VarDecl.h"

#include "llvm/Support/Casting.h"

namespace AST {
    auto Context::addDecl(AST::Decl *const Decl) noexcept -> bool {
        Decl->setLinkage(Decl::Linkage::External);
        return DeclMap.insert({ Decl->getName(), Decl }).second;
    }

    void
    Context::visitStmt(AST::Stmt *const Stmt,
                       ADT::SymbolTable &ParentSymbolTable,
                       Interface::DiagnosticsEngine &Diag,
                       const VisitOptions Options) const noexcept
    {
        if (const auto CompoundStmt = llvm::dyn_cast<AST::CompoundStmt>(Stmt)) {
            return visitCompountStmt(CompoundStmt,
                                     ParentSymbolTable,
                                     Diag,
                                     Options);
        }

        if (const auto FuncDecl = llvm::dyn_cast<AST::FunctionDecl>(Stmt)) {
            return visitFunctionDecl(FuncDecl, ParentSymbolTable, Diag, Options);
        }
    }

    void
    Context::visitCompountStmt(AST::CompoundStmt *const CompoundStmt,
                               ADT::SymbolTable &ParentSymbolTable,
                               Interface::DiagnosticsEngine &Diag,
                               VisitOptions Options) const noexcept
    {
        auto SymbolTable = ADT::SymbolTable(ParentSymbolTable);
        for (const auto &Stmt : CompoundStmt->getStmtList()) {
            visitStmt(Stmt, SymbolTable, Diag, Options);
        }
    }

    void
    Context::visitFunctionDecl(AST::FunctionDecl *const FuncDecl,
                               ADT::SymbolTable &ParentSymbolTable,
                               Interface::DiagnosticsEngine &Diag,
                               const VisitOptions Options) const noexcept
    {
        auto SymbolTable = ADT::SymbolTable(ParentSymbolTable);
        const auto Proto = FuncDecl->getPrototype();

        for (const auto &Param : Proto->getParamList()) {
            if (ParentSymbolTable.hasDecl(Param.getName())) {
                Diag.emitError("\"" SV_FMT "\" is already defined",
                               SV_FMT_ARG(Param.getName()));
                return;
            }
        }

        const auto Body = FuncDecl->getBody();
        if (const auto CompoundStmt = llvm::dyn_cast<AST::CompoundStmt>(Body)) {
            if (CompoundStmt->getStmtList().empty()) {
                CompoundStmt->getStmtListRef().push_back(
                    new AST::ReturnStmt(AST::ReturnStmt::none()));
            }
        }

        visitStmt(Body, SymbolTable, Diag, Options);
    }

    void
    Context::visitVarDecl(AST::VarDecl *const VarDecl,
                          ADT::SymbolTable &ParentSymbolTable,
                          Interface::DiagnosticsEngine &Diag,
                          const VisitOptions Options) const noexcept
    {
        if (ParentSymbolTable.hasDecl(VarDecl->getName())) {
            Diag.emitError("\"" SV_FMT "\" is already defined",
                            SV_FMT_ARG(VarDecl->getName()));
            return;
        }

        ParentSymbolTable.addDecl(VarDecl);
    }

    void
    Context::visitDecls(Interface::DiagnosticsEngine &Diag,
                        const VisitOptions Options) const noexcept
    {
        auto GlobalSymbolTable = ADT::SymbolTable();
        for (auto &[Name, Decl] : getDeclMap()) {
            if (GlobalSymbolTable.hasDecl(Name)) {
                Diag.emitError("\"" SV_FMT "\" is already defined",
                               SV_FMT_ARG(Name));
                return;
            }

            GlobalSymbolTable.addDecl(Decl);
            visitStmt(Decl, GlobalSymbolTable, Diag, Options);
        }
    }
}