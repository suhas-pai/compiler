/*
 * Backend/LLVM/Jit.h
 */

#pragma once
#include <memory>

#include "Backend/LLVM/Handler.h"
#include "Interface/DiagnosticsEngine.h"
#include "llvm/ADT/StringRef.h"

#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/ExecutorProcessControl.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"

namespace Backend::LLVM {
    struct JITHandler : public Handler {
    protected:
        std::unique_ptr<llvm::orc::ExecutionSession> ES;

        llvm::DataLayout DL;
        llvm::orc::MangleAndInterner Mangle;

        llvm::orc::RTDyldObjectLinkingLayer ObjectLayer;
        llvm::orc::IRCompileLayer CompileLayer;

        llvm::orc::JITDylib &MainJD;

        explicit
        JITHandler(std::unique_ptr<llvm::orc::ExecutionSession> ES,
                   llvm::orc::JITTargetMachineBuilder JTMB,
                   llvm::DataLayout DL,
                   Interface::DiagnosticsEngine &Diag) noexcept;

        void AllocCoreFields(const llvm::StringRef &Name) noexcept override;
    public:
        static auto Create(Interface::DiagnosticsEngine &Diag)
            -> std::unique_ptr<JITHandler>;

        virtual ~JITHandler() noexcept;

        [[nodiscard]] constexpr auto &getDataLayout() const noexcept {
            return DL;
        }

        [[nodiscard]] constexpr auto &getMainJITDylib() const noexcept {
            return MainJD;
        }

        [[nodiscard]] auto
        addModule(llvm::orc::ThreadSafeModule TSM,
                  llvm::orc::ResourceTrackerSP RT = nullptr) noexcept
        {
            if (RT == nullptr)
                RT = MainJD.getDefaultResourceTracker();

            return CompileLayer.add(RT, std::move(TSM));
        }

        [[nodiscard]] auto lookup(const llvm::StringRef Name) noexcept
            -> llvm::Expected<llvm::JITEvaluatedSymbol>
        {
            return ES->lookup({&MainJD}, Mangle(Name.str()));
        }

        void
        evalulateAndPrint(AST::Stmt &Stmt,
                          std::string_view Prefix = "",
                          std::string_view Suffix = "") noexcept override;
    };
}