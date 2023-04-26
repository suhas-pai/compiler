/*
 * Backend/LLVM/Handler.cpp
 */

#include <cstdio>
#include <memory>

#include "AST/Expr.h"
#include "Backend/LLVM/Handler.h"

#include "llvm/IR/Constants.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar.h"

namespace Backend::LLVM {
    void Handler::LLVMInitialize() noexcept {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmParser();
        llvm::InitializeNativeTargetAsmPrinter();
    }

    void Handler::AllocCoreFields(const llvm::StringRef &Name) noexcept {
        this->TheContext = std::make_unique<llvm::LLVMContext>();
        this->Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
        this->TheModule = std::make_unique<llvm::Module>(Name, *TheContext);
    }

    void Handler::Initialize(const llvm::StringRef &Name) noexcept {
        // Create a new pass manager attached to it.
        AllocCoreFields("Simple");

        FPM =
            std::make_unique<llvm::legacy::FunctionPassManager>(
                TheModule.get());

        // Do simple "peephole" optimizations and bit-twiddling optzns.
        FPM->add(llvm::createInstructionCombiningPass());
        // Reassociate expressions.
        FPM->add(llvm::createReassociatePass());
        // Eliminate Common SubExpressions.
        FPM->add(llvm::createGVNPass());
        // Simplify the control flow graph (deleting unreachable blocks, etc).
        FPM->add(llvm::createCFGSimplificationPass());

        FPM->doInitialization();
    }

    Handler::Handler(const llvm::StringRef &Name) noexcept
    {
        Initialize("Simple");
    }

    Handler::Handler() noexcept : Handler("Handler") {}

    auto Handler::findFunction(const std::string_view Name) const noexcept
        -> llvm::Function *
    {
        const auto Doubles =
            std::vector(2, llvm::Type::getDoubleTy(*TheContext));
        const auto FT =
            llvm::FunctionType::get(llvm::Type::getDoubleTy(*TheContext),
                                    Doubles,
                                    /*isVarArg=*/false);

        return
            llvm::Function::Create(FT,
                                   llvm::Function::ExternalLinkage,
                                   "pow",
                                   TheModule.get());
    }

    void Handler::evalulateAndPrint(AST::Expr &Expr) noexcept {
        const auto Codegen = Expr.codegen(*this);
        if (const auto Constant = llvm::dyn_cast<llvm::ConstantFP>(Codegen)) {
            fprintf(stdout, "%f", Constant->getValueAPF().convertToDouble());
        } else if (const auto Constant =
                        llvm::dyn_cast<llvm::ConstantInt>(Codegen))
        {
            fprintf(stdout, "%" PRId64, Constant->getSExtValue());
        }
    }
}