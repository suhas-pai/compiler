/*
 * Backend/LLVM/Simple.h
 */

#pragma once

#include "ADT/StringMap.h"
#include "AST/Context.h"
#include "Interface/DiagnosticsEngine.h"

#include "llvm/ExecutionEngine/JITSymbol.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/User.h"

#include "llvm/Support/Error.h"

namespace AST {
    struct Stmt;
    struct Decl;
    struct Expr;
    struct FunctionDecl;
}

namespace Backend::LLVM {
    struct ValueMap {
    protected:
        ADT::UnorderedStringMap<std::vector<llvm::Value *>> Map;
    public:
        auto addValue(std::string_view Name, llvm::Value *Val) noexcept
            -> decltype(*this);

        auto setValue(std::string_view Name, llvm::Value *Val) noexcept
            -> decltype(*this);

        auto getValue(std::string_view Name) const noexcept -> llvm::Value *;
        auto removeValue(std::string_view Name) noexcept
            -> decltype(*this);
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
        std::vector<AST::Decl *> DeclList;

        std::unique_ptr<llvm::legacy::FunctionPassManager> FPM;
        llvm::ExitOnError ExitOnErr;

        Interface::DiagnosticsEngine *Diag;

        static void LLVMInitialize() noexcept;

        virtual void AllocCoreFields(const llvm::StringRef &Name) noexcept;

        void Initialize(const llvm::StringRef &Name) noexcept;
        auto createModuleAndFPM() ->
            std::pair<std::unique_ptr<llvm::Module>,
                      std::unique_ptr<llvm::legacy::PassManager>>;

        explicit
        Handler(const llvm::StringRef &ModuleName,
                Interface::DiagnosticsEngine *Diag) noexcept;
    public:
        explicit Handler(Interface::DiagnosticsEngine *Diag) noexcept;

        [[nodiscard]] constexpr auto &getContext() noexcept {
            return *TheContext;
        }

        [[nodiscard]] constexpr auto &getModule() const noexcept {
            return *TheModule;
        }

        [[nodiscard]] constexpr auto &getBuilder() noexcept {
            return *Builder;
        }

        [[nodiscard]] constexpr auto &getNameToASTNodeMap() const noexcept {
            return NameToASTNode;
        }

        [[nodiscard]] constexpr auto &getNameToASTNodeMapRef() noexcept {
            return NameToASTNode;
        }

        [[nodiscard]] constexpr auto &getDeclList() const noexcept {
            return DeclList;
        }

        [[nodiscard]] constexpr auto &getDeclListRef() noexcept {
            return DeclList;
        }

        [[nodiscard]] constexpr auto &getFPM() const noexcept {
            return *FPM;
        }

        [[nodiscard]] constexpr auto &getDiag() const noexcept {
            return Diag;
        }

        constexpr
        auto setDiag(Interface::DiagnosticsEngine *const Diag) noexcept
            -> decltype(*this)
        {
            this->Diag = Diag;
            return *this;
        }

        auto addASTNode(std::string_view Name, AST::Stmt &Node) noexcept
            -> decltype(*this);

        auto getASTNode(std::string_view Name) noexcept -> AST::Stmt *;
        auto removeASTNode(std::string_view Name) noexcept -> decltype(*this);

        virtual bool evalulate(AST::Context &Context) noexcept;
    };
}