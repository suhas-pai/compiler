/*
 * Backend/LLVM/Handler.cpp
 */

#include <cstdio>
#include <memory>

#include "AST/Expr.h"
#include "AST/FunctionDecl.h"
#include "Backend/LLVM/Handler.h"

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
                     Interface::DiagnosticsEngine *const Diag) noexcept
    : Diag(Diag)
    {
        Initialize("Simple");
    }

    Handler::Handler(Interface::DiagnosticsEngine *Diag) noexcept
    : Handler("Handler", Diag) {}

    auto
    ValueMap::addValue(const std::string_view Name,
                       llvm::Value *const Value) noexcept
        -> decltype(*this)
    {
        if (const auto Iter = this->find(Name); Iter != this->end()) {
            Iter->second.push_back(Value);
        }

        this->insert({ std::string(Name), std::vector({Value}) });
        return *this;
    }

    auto ValueMap::getValue(const std::string_view Name) const noexcept
        -> llvm::Value *
    {
        if (const auto Iter = this->find(Name); Iter != this->end()) {
            return Iter->second.back();
        }

        return nullptr;
    }

    auto
    ValueMap::removeValue(const std::string_view Name) noexcept
        -> decltype(*this)
    {
        if (const auto Iter = this->find(Name); Iter != this->end()) {
            Iter->second.pop_back();
            if (Iter->second.empty()) {
                this->erase(Iter);
            }
        }

        return *this;
    }

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
                                   Name,
                                   TheModule.get());
    }

    auto
    Handler::addASTNode(const std::string_view Name,
                        AST::Stmt &Node) noexcept
        -> decltype(*this)
    {
        getNameToASTNodeMapRef().insert({
            std::string(Name),
            &Node
        });

        if (const auto Decl = llvm::dyn_cast<AST::Decl>(&Node)) {
            getDeclListRef().emplace_back(Decl);
        }

        return *this;
    }

    auto Handler::getASTNode(const std::string_view Name) noexcept
        -> AST::Stmt *
    {
        const auto Iter = getNameToASTNodeMap().find(std::string(Name));
        if (Iter == getNameToASTNodeMap().end()) {
            return nullptr;
        }

        return Iter->second;
    }

    auto Handler::removeASTNode(const std::string_view Name) noexcept
        -> decltype(*this)
    {
        this->NameToASTNode.erase(std::string(Name));
        return *this;
    }

    bool
    Handler::evalulateAndPrint(AST::Stmt &Stmt,
                               const std::string_view Prefix,
                               const std::string_view Suffix) noexcept
    {
        auto ValueMap = ::Backend::LLVM::ValueMap();
        const auto Codegen = Stmt.codegen(*this, ValueMap);

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

        return true;
    }
}