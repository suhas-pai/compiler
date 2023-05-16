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
        AllocCoreFields("Compiler");

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
        Initialize("Compiler");
    }

    Handler::Handler(Interface::DiagnosticsEngine *Diag) noexcept
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
        getNameToASTNodeMapRef().insert({
            std::string(Name),
            &Node
        });

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
        this->getNameToASTNodeMapRef().erase(std::string(Name));
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
                ValueMap.addValue(Proto->getName(), ProtoCodegen);

                const auto FinishedValueOpt =
                    FuncDecl->finishPrototypeCodegen(*this,
                                                     getBuilder(),
                                                     ValueMap,
                                                     ProtoCodegen);

                if (!FinishedValueOpt.has_value()) {
                    return false;
                }

                ValueMap.setValue(Proto->getName(), FinishedValueOpt.value());
                continue;
            }

            if (const auto ValueOpt =
                    Decl->codegen(*this, getBuilder(), ValueMap))
            {
                addASTNode(Name, *Decl);
                ValueMap.addValue(Decl->getName(), ValueOpt.value());

                continue;
            }

            return false;
        }

        return true;
    }
}