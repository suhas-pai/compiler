/*
 * Backend/LLVM/Codegen.h
 */

#pragma once

#include "AST/BinaryOperation.h"
#include "AST/CharLiteral.h"
#include "AST/CompoundStmt.h"
#include "AST/FunctionDecl.h"
#include "AST/FunctionCall.h"
#include "AST/IfStmt.h"
#include "AST/NumberLiteral.h"
#include "AST/ParenExpr.h"
#include "AST/ReturnStmt.h"
#include "AST/StringLiteral.h"
#include "AST/UnaryOperation.h"
#include "AST/VarDecl.h"
#include "AST/VariableRef.h"

#include "llvm/IR/Value.h"

namespace Backend::LLVM {
    auto
    BinaryOperationCodegen(AST::BinaryOperation &BinOp,
                           Backend::LLVM::Handler &Handler,
                           llvm::IRBuilder<> &Builder,
                           Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;

    auto
    CharLiteralCodegen(AST::CharLiteral &CharLit,
                       Backend::LLVM::Handler &Handler,
                       llvm::IRBuilder<> &Builder,
                       Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;

    auto
    CompoundStmtCodegen(AST::CompoundStmt &CompountStmt,
                        Backend::LLVM::Handler &Handler,
                        llvm::IRBuilder<> &Builder,
                        Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;

    auto
    FunctionCallCodegen(AST::FunctionCall &FuncCall,
                        Backend::LLVM::Handler &Handler,
                        llvm::IRBuilder<> &Builder,
                        Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;

    auto
    FunctionDeclFinishPrototypeCodegen(AST::FunctionDecl &FuncDecl,
                                       Backend::LLVM::Handler &Handler,
                                       llvm::IRBuilder<> &Builder,
                                       Backend::LLVM::ValueMap &ValueMap,
                                       llvm::Value *ProtoCodegen,
                                       llvm::BasicBlock *BB) noexcept
        -> std::optional<llvm::Value *>;

    auto
    FunctionPrototypeCodegen(AST::FunctionPrototype &FuncProto,
                             Backend::LLVM::Handler &Handler,
                             llvm::IRBuilder<> &Builder,
                             Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;

    auto
    FunctionDeclCodegen(AST::FunctionDecl &FuncDecl,
                        Backend::LLVM::Handler &Handler,
                        llvm::IRBuilder<> &Builder,
                        Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;

    auto
    IfStmtCodegen(AST::IfStmt &IfStmt,
                  Backend::LLVM::Handler &Handler,
                  llvm::IRBuilder<> &Builder,
                  Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;

    auto
    NumberLiteralCodegen(AST::NumberLiteral &NumLit,
                         Backend::LLVM::Handler &Handler,
                         llvm::IRBuilder<> &Builder,
                         Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;

    auto
    ParenExprCodegen(AST::ParenExpr &ParenExpr,
                     Backend::LLVM::Handler &Handler,
                     llvm::IRBuilder<> &Builder,
                     Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;

    auto
    ReturnStmtCodegen(AST::ReturnStmt &RetStmt,
                      Backend::LLVM::Handler &Handler,
                      llvm::IRBuilder<> &Builder,
                      Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;

    auto
    StringLiteralCodegen(AST::StringLiteral &StrLit,
                         Backend::LLVM::Handler &Handler,
                         llvm::IRBuilder<> &Builder,
                         Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;

    auto
    UnaryOperationCodegen(AST::UnaryOperation &UnaryOp,
                          Backend::LLVM::Handler &Handler,
                          llvm::IRBuilder<> &Builder,
                          Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;

    auto
    VarDeclCodegen(AST::VarDecl &VarDecl,
                   Backend::LLVM::Handler &Handler,
                   llvm::IRBuilder<> &Builder,
                   Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;

    auto
    VariableRefCodegen(AST::VariableRef &VarRef,
                       Backend::LLVM::Handler &Handler,
                       llvm::IRBuilder<> &Builder,
                       Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>;
}