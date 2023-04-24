/*
 * Backend/LLVM/Vars.h
 */

#pragma once

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

// This is an object that owns LLVM core data structures
extern llvm::LLVMContext TheContext;

// This is a helper object that makes easy to generate LLVM instructions
extern llvm::IRBuilder<> Builder;

// This is an LLVM construct that contains functions and global variables
extern std::unique_ptr<llvm::Module> TheModule;

// This map keeps track of which values are defined in the current scope
extern std::unordered_map<std::string, llvm::Value *> NamedValues;

extern llvm::Function *PowerFunc;

void SetupLLVMBackend() noexcept;