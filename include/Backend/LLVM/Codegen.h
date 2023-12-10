/*
 * Backend/LLVM/Codegen.h
 */

#pragma once

#include "AST/FunctionDecl.h"
#include "llvm/IR/Value.h"

namespace Backend::LLVM {
    auto
    FunctionDeclFinishPrototypeCodegen(AST::FunctionDecl &FuncDecl,
                                       Backend::LLVM::Handler &Handler,
                                       llvm::IRBuilder<> &Builder,
                                       Backend::LLVM::ValueMap &ValueMap,
                                       llvm::Value *const ProtoCodegen) noexcept
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
}