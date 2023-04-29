/*
 * Backend/LLVM/Simple.h
 */

#pragma once

#include "Basic/StringHash.h"
#include "Interface/DiagnosticsEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/User.h"

#include "llvm/Support/Error.h"

namespace AST {
    struct Expr;
}

namespace Backend::LLVM {
    struct Handler {
    protected:
        // This is an object that owns LLVM core data structures
        std::unique_ptr<llvm::LLVMContext> TheContext;

        // This is a helper object that makes easy to generate LLVM instructions
        std::unique_ptr<llvm::IRBuilder<>> Builder;

        // This is an LLVM construct that contains functions and global variables
        std::unique_ptr<llvm::Module> TheModule;

        // This map keeps track of which values are defined in the current scope
        std::unordered_map<std::string,
                           AST::Expr *,
                           StringHash,
                           std::equal_to<>> NamedValues;

        std::unique_ptr<llvm::legacy::FunctionPassManager> FPM;
        llvm::ExitOnError ExitOnErr;

        Interface::DiagnosticsEngine &Diag;

        static void LLVMInitialize() noexcept;

        virtual void AllocCoreFields(const llvm::StringRef &Name) noexcept;
        void Initialize(const llvm::StringRef &Name) noexcept;

        explicit
        Handler(const llvm::StringRef &ModuleName,
                Interface::DiagnosticsEngine &Diag) noexcept;
    public:
        explicit Handler(Interface::DiagnosticsEngine &Diag) noexcept;

        [[nodiscard]] constexpr auto &getContext() noexcept {
            return *TheContext;
        }

        [[nodiscard]] constexpr auto &getModule() const noexcept {
            return *TheModule;
        }

        [[nodiscard]] constexpr auto &getBuilder() noexcept {
            return *Builder;
        }

        [[nodiscard]] constexpr auto &getNamedValues() const noexcept {
            return NamedValues;
        }

        [[nodiscard]] constexpr auto &getNamedValuesRef() noexcept {
            return NamedValues;
        }

        [[nodiscard]] constexpr auto &getFPM() const noexcept {
            return *FPM;
        }

        [[nodiscard]] constexpr auto &getDiag() const noexcept {
            return Diag;
        }

        [[nodiscard]]
        auto getValueForName(const std::string_view Name) noexcept
            -> llvm::Value *;

        [[nodiscard]]
        auto findFunction(std::string_view Name) const noexcept
            -> llvm::Function *;

        virtual void
        evalulateAndPrint(AST::Expr &Expr,
                          std::string_view Prefix = "",
                          std::string_view Suffix = "") noexcept;
    };
}