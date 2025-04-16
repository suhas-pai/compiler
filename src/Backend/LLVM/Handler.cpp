/*
 * Backend/LLVM/Handler.cpp
 */

#include <memory>

#include "Backend/LLVM/Handler.h"
#include "Backend/LLVM/Codegen.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/TargetSelect.h"

#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"

namespace Backend::LLVM {
    void Handler::initializeLLVM() noexcept {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmParser();
        llvm::InitializeNativeTargetAsmPrinter();
    }

    void Handler::allocCoreFields(const llvm::StringRef &Name) noexcept {
        // Open a new context and module.
        this->TheContext = std::make_unique<llvm::LLVMContext>();
        this->TheModule =
            std::make_unique<llvm::Module>("KaleidoscopeJIT", *TheContext);
    }

    void Handler::initialize(const llvm::StringRef &Name) noexcept {
        this->allocCoreFields("Compiler");
        this->Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);

        // Create new pass and analysis managers.
        this->FPM = std::make_unique<llvm::FunctionPassManager>();
        this->LAM = std::make_unique<llvm::LoopAnalysisManager>();
        this->FAM = std::make_unique<llvm::FunctionAnalysisManager>();
        this->CGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
        this->MAM = std::make_unique<llvm::ModuleAnalysisManager>();
        this->PIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
        this->SI =
            std::make_unique<llvm::StandardInstrumentations>(
                *TheContext,
                /*DebugLogging=*/true);

        this->SI->registerCallbacks(*PIC, MAM.get());
        // Add transform passes.
        // Do simple "peephole" optimizations and bit-twiddling optzns.
        this->FPM->addPass(llvm::InstCombinePass());
        // Re-associate expressions.
        this->FPM->addPass(llvm::ReassociatePass());
        // Eliminate Common SubExpressions.
        this->FPM->addPass(llvm::GVNPass());
        // Simplify the control flow graph (deleting unreachable blocks, etc).
        this->FPM->addPass(llvm::SimplifyCFGPass());

        // Register analysis passes used in these transform passes.
        auto PB = llvm::PassBuilder();

        PB.registerModuleAnalyses(*MAM);
        PB.registerFunctionAnalyses(*FAM);
        PB.crossRegisterProxies(*LAM, *FAM, *CGAM, *MAM);
    }

    Handler::Handler(const llvm::StringRef &Name,
                     DiagnosticConsumer &Diag) noexcept
    : Diag(Diag)
    {
        this->initialize("Compiler");
    }

    Handler::Handler(DiagnosticConsumer  &Diag) noexcept
    : Handler("Compiler", Diag) {}

    auto
    ValueMap::add(const std::string_view Name,
                       llvm::Value *const Value) noexcept -> decltype(*this)
    {
        if (const auto Iter = this->Map.find(Name); Iter != this->Map.end()) {
            Iter->second.push_back(Value);
        } else {
            this->Map.insert({ std::string(Name), std::vector({Value}) });
        }

        return *this;
    }

    auto
    ValueMap::setValue(const std::string_view Name,
                       llvm::Value *const Value) noexcept -> decltype(*this)
    {
        if (const auto Iter = this->Map.find(Name); Iter != this->Map.end()) {
            Iter->second.clear();
            Iter->second.push_back(Value);

            return *this;
        }

        this->Map.insert({ std::string(Name), std::vector({Value}) });
        return *this;
    }

    auto ValueMap::getValue(const std::string_view Name) const noexcept
        -> llvm::Value *
    {
        if (const auto Iter = this->Map.find(Name); Iter != this->Map.end()) {
            return Iter->second.back();
        }

        return nullptr;
    }

    auto ValueMap::removeValue(const std::string_view Name) noexcept
        -> decltype(*this)
    {
        if (const auto Iter = this->Map.find(Name); Iter != this->Map.end()) {
            Iter->second.pop_back();
            if (Iter->second.empty()) {
                Map.erase(Iter);
            }
        }

        return *this;
    }

    auto ValueMap::clear() noexcept -> decltype(*this) {
        this->Map.clear();
        return *this;
    }

    auto
    Handler::addASTNode(const std::string_view Name, AST::Stmt &Node) noexcept
        -> decltype(*this)
    {
        this->getNameToASTNodeMapRef().insert({ std::string(Name), &Node });
        return *this;
    }

    auto Handler::getASTNode(const std::string_view Name) noexcept
        -> AST::Stmt *
    {
        const auto Iter = this->getNameToASTNodeMap().find(Name);
        if (Iter == this->getNameToASTNodeMap().end()) {
            return nullptr;
        }

        return Iter->second;
    }

    auto Handler::removeASTNode(const std::string_view Name) noexcept
        -> decltype(*this)
    {
        auto &Map = this->getNameToASTNodeMapRef();
        if (const auto Iter = Map.find(Name); Iter != Map.end()) {
            Map.erase(Iter);
        }

        return *this;
    }

    bool Handler::evaluate(Parse::ParseUnit &Unit) noexcept {
        auto ValueMap = LLVM::ValueMap();
        for (const auto &[Name, Decl] : Unit.getTopLevelDeclList()) {
            // We need to be able to reference a function within its body,
            // so this case must be handled differently.

            if (const auto FuncDecl = llvm::dyn_cast<AST::FunctionDecl>(Decl)) {
                const auto FuncDeclCodegenOpt =
                    FunctionDeclCodegen(*FuncDecl,
                                        *this,
                                        getBuilder(),
                                        ValueMap);

                if (!FuncDeclCodegenOpt.has_value()) {
                    return false;
                }

                ValueMap.add(Name, FuncDeclCodegenOpt.value());
                continue;
            }

            if (const auto ValueOpt =
                    this->codegen(*Decl, this->getBuilder(), ValueMap))
            {
                addASTNode(Name, *Decl);
                ValueMap.add(Name, ValueOpt.value());

                continue;
            }

            return false;
        }

        return true;
    }
}