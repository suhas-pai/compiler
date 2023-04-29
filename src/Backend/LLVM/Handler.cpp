/*
 * Backend/LLVM/Handler.cpp
 */

#include <cstdio>
#include <memory>

#include "AST/Expr.h"
#include "Backend/LLVM/Handler.h"
#include "Basic/Macros.h"

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

    Handler::Handler(const llvm::StringRef &Name,
                     Interface::DiagnosticsEngine &Diag) noexcept : Diag(Diag)
    {
        Initialize("Simple");
    }

    Handler::Handler(Interface::DiagnosticsEngine &Diag) noexcept
    : Handler("Handler", Diag) {}

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

    auto Handler::getValueForName(const std::string_view Name) noexcept
        -> llvm::Value *
    {
        if (const auto Iter = NamedValues.find(Name);
            Iter != NamedValues.end())
        {
            return Iter->second->codegen(*this);
        }

        return nullptr;
    }

    void
    Handler::evalulateAndPrint(AST::Expr &Expr,
                               const std::string_view Prefix,
                               const std::string_view Suffix) noexcept
    {
        const auto Codegen = Expr.codegen(*this);
        if (const auto Constant = llvm::dyn_cast<llvm::ConstantFP>(Codegen)) {
            fprintf(stdout,
                    SV_FMT "%f" SV_FMT,
                    SV_FMT_ARG(Prefix),
                    Constant->getValueAPF().convertToDouble(),
                    SV_FMT_ARG(Suffix));
        } else if (const auto Constant =
                    llvm::dyn_cast<llvm::ConstantInt>(Codegen))
        {
            fprintf(stdout,
                    SV_FMT "%" PRId64 SV_FMT,
                    SV_FMT_ARG(Prefix),
                    Constant->getSExtValue(),
                    SV_FMT_ARG(Suffix));
        }
    }
}