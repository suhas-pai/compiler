/*
 * Backend/LLVM/Simple.h
 */

#pragma once
#include <span>

#include "ADT/StringMap.h"
#include "AST/Decls/LvalueNamedDecl.h"

#include "Diag/Consumer.h"
#include "Parse/ParseUnit.h"

#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Analysis/LoopAnalysisManager.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Module.h"

#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/Error.h"

namespace Backend::LLVM {
    struct ValueMap {
    protected:
        ADT::UnorderedStringMap<std::vector<llvm::Value *>> Map;
    public:
        auto add(std::string_view Name, llvm::Value *Val) noexcept
            -> decltype(*this);
        auto setValue(std::string_view Name, llvm::Value *Val) noexcept
            -> decltype(*this);

        auto getValue(std::string_view Name) const noexcept -> llvm::Value *;
        auto removeValue(std::string_view Name) noexcept -> decltype(*this);

        auto clear() noexcept -> decltype(*this);
    };

    // Handler is a struct that handles the llvm-backend process to create a
    // program.
    // This program shares a single 'module' in llvm parlance, and a single
    // function-pass-manager that optimizes the program.

    struct Handler {
    protected:
        // This is an object that owns LLVM core data structures
        std::unique_ptr<llvm::LLVMContext> TheContext;

        // This is a helper object that makes easy to generate LLVM instructions
        std::unique_ptr<llvm::IRBuilder<>> Builder;

        // This is an LLVM construct that contains functions and global variables
        std::unique_ptr<llvm::Module> TheModule;

        // This map keeps track of which values are defined in the current scope

        ADT::UnorderedStringMap<AST::Stmt *> NameToASTNode;
        std::vector<AST::LvalueNamedDecl *> DeclList;

        std::unique_ptr<llvm::FunctionPassManager> FPM;
        std::unique_ptr<llvm::LoopAnalysisManager> LAM;
        std::unique_ptr<llvm::FunctionAnalysisManager> FAM;
        std::unique_ptr<llvm::CGSCCAnalysisManager> CGAM;
        std::unique_ptr<llvm::ModuleAnalysisManager> MAM;
        std::unique_ptr<llvm::PassInstrumentationCallbacks> PIC;
        std::unique_ptr<llvm::StandardInstrumentations> SI;

        llvm::ExitOnError ExitOnErr;
        DiagnosticConsumer &Diag;

        static void initializeLLVM() noexcept;
        virtual void allocCoreFields(const llvm::StringRef &Name) noexcept;

        void initialize(const llvm::StringRef &Name) noexcept;

        explicit
        Handler(const llvm::StringRef &ModuleName,
                DiagnosticConsumer &Diag) noexcept;
    public:
        explicit Handler(DiagnosticConsumer &Diag) noexcept;

        [[nodiscard]] constexpr auto &getContext() noexcept {
            return *this->TheContext;
        }

        [[nodiscard]] constexpr auto &getModule() const noexcept {
            return *this->TheModule;
        }

        [[nodiscard]] constexpr auto &getBuilder() noexcept {
            return *this->Builder;
        }

        [[nodiscard]] constexpr auto &getNameToASTNodeMap() const noexcept {
            return this->NameToASTNode;
        }

        [[nodiscard]] constexpr auto &getNameToASTNodeMapRef() noexcept {
            return this->NameToASTNode;
        }

        [[nodiscard]] constexpr auto getDeclList() const noexcept {
            return std::span(this->DeclList);
        }

        [[nodiscard]] constexpr auto &getDeclListRef() noexcept {
            return this->DeclList;
        }

        [[nodiscard]] constexpr auto &getFPM() const noexcept {
            return *this->FPM;
        }

        [[nodiscard]] constexpr auto &getLAM() const noexcept {
            return *this->LAM;
        }

        [[nodiscard]] constexpr auto &getFAM() const noexcept {
            return *this->FAM;
        }

        [[nodiscard]] constexpr auto &getCGAM() const noexcept {
            return *this->CGAM;
        }
        [[nodiscard]] constexpr auto &getMAM() const noexcept {
            return *this->MAM;
        }
        [[nodiscard]] constexpr auto &getPIC() const noexcept {
            return *this->PIC;
        }
        [[nodiscard]] constexpr auto &getSI() const noexcept {
            return *this->SI;
        }

        [[nodiscard]] constexpr auto &getDiag() const noexcept {
            return this->Diag;
        }

        auto addASTNode(std::string_view Name, AST::Stmt &Node) noexcept
            -> decltype(*this);

        auto getASTNode(std::string_view Name) noexcept -> AST::Stmt *;
        auto removeASTNode(std::string_view Name) noexcept -> decltype(*this);

        virtual std::optional<llvm::Value *>
        codegen(AST::Stmt &Stmt,
                llvm::IRBuilder<> &Builder,
                LLVM::ValueMap &ValueMap) noexcept;

        virtual bool evaluate(Parse::ParseUnit &Context) noexcept;
    };
}