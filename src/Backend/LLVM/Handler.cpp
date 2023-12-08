/*
 * Backend/LLVM/Handler.cpp
 */

#include <memory>

#include "AST/FunctionDecl.h"

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
        TheContext = std::make_unique<llvm::LLVMContext>();
        TheModule =
            std::make_unique<llvm::Module>("KaleidoscopeJIT", *TheContext);
    }

    void Handler::initialize(const llvm::StringRef &Name) noexcept {
        // Create a new pass manager attached to it.
        allocCoreFields("Compiler");

        Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);

        // Create new pass and analysis managers.
        FPM = std::make_unique<llvm::FunctionPassManager>();
        FPM = std::make_unique<llvm::FunctionPassManager>();
        LAM = std::make_unique<llvm::LoopAnalysisManager>();
        FAM = std::make_unique<llvm::FunctionAnalysisManager>();
        CGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
        MAM = std::make_unique<llvm::ModuleAnalysisManager>();
        PIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
        SI =
            std::make_unique<llvm::StandardInstrumentations>(
                *TheContext,
                /*DebugLogging*/true);

        SI->registerCallbacks(*PIC, MAM.get());
        // Add transform passes.
        // Do simple "peephole" optimizations and bit-twiddling optzns.
        FPM->addPass(llvm::InstCombinePass());
        // Reassociate expressions.
        FPM->addPass(llvm::ReassociatePass());
        // Eliminate Common SubExpressions.
        FPM->addPass(llvm::GVNPass());
        // Simplify the control flow graph (deleting unreachable blocks, etc).
        FPM->addPass(llvm::SimplifyCFGPass());

        // Register analysis passes used in these transform passes.
        llvm::PassBuilder PB;
        PB.registerModuleAnalyses(*MAM);
        PB.registerFunctionAnalyses(*FAM);
        PB.crossRegisterProxies(*LAM, *FAM, *CGAM, *MAM);
    }

    Handler::Handler(const llvm::StringRef &Name,
                     Interface::DiagnosticsEngine &Diag) noexcept
    : Diag(Diag)
    {
        initialize("Compiler");
    }

    Handler::Handler(Interface::DiagnosticsEngine &Diag) noexcept
    : Handler("Compiler", Diag) {}

    auto
    ValueMap::addValue(const std::string_view Name,
                       llvm::Value *const Value) noexcept
        -> decltype(*this)
    {
        if (const auto Iter = Map.find(Name); Iter != Map.end()) {
            Iter->second.push_back(Value);
        } else {
            Map.insert({ std::string(Name), std::vector({Value}) });
        }

        return *this;
    }

    auto
    ValueMap::setValue(const std::string_view Name,
                       llvm::Value *const Value) noexcept
        -> decltype(*this)
    {
        if (const auto Iter = Map.find(Name); Iter != Map.end()) {
            Iter->second.pop_back();
            Iter->second.push_back(Value);

            return *this;
        }

        Map.insert({ std::string(Name), std::vector({Value}) });
        return *this;
    }

    auto ValueMap::getValue(const std::string_view Name) const noexcept
        -> llvm::Value *
    {
        if (const auto Iter = Map.find(Name); Iter != Map.end()) {
            return Iter->second.back();
        }

        return nullptr;
    }

    auto ValueMap::removeValue(const std::string_view Name) noexcept
        -> decltype(*this)
    {
        if (const auto Iter = Map.find(Name); Iter != Map.end()) {
            Iter->second.pop_back();
            if (Iter->second.empty()) {
                Map.erase(Iter);
            }
        }

        return *this;
    }

    auto
    Handler::addASTNode(const std::string_view Name, AST::Stmt &Node) noexcept
        -> decltype(*this)
    {
        getNameToASTNodeMapRef().insert({ std::string(Name), &Node });
        return *this;
    }

    auto Handler::getASTNode(const std::string_view Name) noexcept
        -> AST::Stmt *
    {
        const auto Iter = getNameToASTNodeMap().find(Name);
        if (Iter == getNameToASTNodeMap().end()) {
            return nullptr;
        }

        return Iter->second;
    }

    auto Handler::removeASTNode(const std::string_view Name) noexcept
        -> decltype(*this)
    {
        getNameToASTNodeMapRef().erase(std::string(Name));
        return *this;
    }

    bool Handler::evalulate(AST::Context &Context) noexcept {
        auto ValueMap = LLVM::ValueMap();
        for (const auto &[Name, Decl] : Context.getDeclMap()) {
            // We need to be able to reference a function within its body,
            // so this case must be handled differently.

            if (const auto FuncDecl = llvm::dyn_cast<AST::FunctionDecl>(Decl)) {
                const auto Proto = FuncDecl->getPrototype();
                const auto ProtoCodegenOpt =
                    Proto->codegen(*this, getBuilder(), ValueMap);

                if (!ProtoCodegenOpt.has_value()) {
                    return false;
                }

                const auto ProtoCodegen = ProtoCodegenOpt.value();

                addASTNode(Name, *Decl);
                ValueMap.addValue(Name, ProtoCodegen);

                const auto FinishedValueOpt =
                    FuncDecl->finishPrototypeCodegen(*this,
                                                     getBuilder(),
                                                     ValueMap,
                                                     ProtoCodegen);

                if (!FinishedValueOpt.has_value()) {
                    return false;
                }

                ValueMap.setValue(Name, FinishedValueOpt.value());
                continue;
            }

            if (const auto ValueOpt =
                    Decl->codegen(*this, getBuilder(), ValueMap))
            {
                addASTNode(Name, *Decl);
                ValueMap.addValue(Name, ValueOpt.value());

                continue;
            }

            return false;
        }

        return true;
    }
}