/*
 * Backend/LLVM/Vars.cpp
 */

#include "Backend/LLVM/Vars.h"

llvm::LLVMContext TheContext;

// This is a helper object that makes easy to generate LLVM instructions
llvm::IRBuilder<> Builder(TheContext);

// This is an LLVM construct that contains functions and global variables
std::unique_ptr<llvm::Module> TheModule;

// This map keeps track of which values are defined in the current scope
std::unordered_map<std::string, llvm::Value *> NamedValues;

llvm::Function *PowerFunc = nullptr;

void SetupLLVMBackend() noexcept {
    const auto Doubles = std::vector(2, llvm::Type::getDoubleTy(TheContext));
    const auto FT =
        llvm::FunctionType::get(llvm::Type::getDoubleTy(TheContext),
                                Doubles,
                                /*isVarArg=*/false);

    PowerFunc =
        llvm::Function::Create(FT,
                               llvm::Function::ExternalLinkage,
                               "_pow",
                               TheModule.get());
}