/*
 * Backend/LLVM/Jit.h
 */

#pragma once

#include <expected>
#include <memory>

#include "Backend/LLVM/Handler.h"
#include "Diag/Consumer.h"
#include "Parse/ParseUnit.h"

#include "llvm/ADT/StringRef.h"

#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/EPCIndirectionUtils.h"
#include "llvm/ExecutionEngine/Orc/ExecutorProcessControl.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/IRTransformLayer.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/Shared/ExecutorSymbolDef.h"

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"

namespace Backend::LLVM {
    struct JITHandler : public Handler {
    protected:
        std::unique_ptr<llvm::orc::ExecutionSession> ES;
        std::unique_ptr<llvm::orc::EPCIndirectionUtils> EPCIU;

        llvm::DataLayout DL;
        llvm::orc::MangleAndInterner Mangle;

        llvm::orc::RTDyldObjectLinkingLayer ObjectLayer;
        llvm::orc::IRCompileLayer CompileLayer;
        llvm::orc::IRTransformLayer OptimizeLayer;

        llvm::orc::JITDylib &MainJD;
        const Parse::ParseUnit &Unit;

        ValueMap ValueMap;

        explicit
        JITHandler(DiagnosticConsumer &Diag,
                   const Parse::ParseUnit &Unit,
                   std::unique_ptr<llvm::orc::ExecutionSession> ES,
                   std::unique_ptr<llvm::orc::EPCIndirectionUtils> EPCIU,
                   llvm::orc::JITTargetMachineBuilder JTMB,
                   llvm::DataLayout DL) noexcept;

        void allocCoreFields(const llvm::StringRef &Name) noexcept override;
    public:
        static auto
        create(DiagnosticConsumer &Diag,
               const Parse::ParseUnit &Unit) noexcept
            -> std::expected<std::unique_ptr<JITHandler>, llvm::Error>;

        virtual ~JITHandler() noexcept;

        [[nodiscard]] constexpr auto &getDataLayout() const noexcept {
            return this->DL;
        }

        [[nodiscard]] constexpr auto &getMainJITDylib() const noexcept {
            return this->MainJD;
        }

        [[nodiscard]] constexpr auto &getUnit() const noexcept {
            return this->Unit;
        }

        [[nodiscard]] auto
        addModule(llvm::orc::ThreadSafeModule TSM,
                  llvm::orc::ResourceTrackerSP RT = nullptr) noexcept
        {
            if (RT == nullptr) {
                RT = this->MainJD.getDefaultResourceTracker();
            }

            return this->CompileLayer.add(RT, std::move(TSM));
        }

        [[nodiscard]] auto lookup(const llvm::StringRef Name) noexcept
            -> llvm::Expected<llvm::orc::ExecutorSymbolDef>
        {
            return this->ES->lookup({&this->MainJD}, Mangle(Name.str()));
        }

        bool
        evaluateAndPrint(AST::Stmt &Stmt,
                         bool PrintIR,
                         std::string_view Prefix = "",
                         std::string_view Suffix = "") noexcept;
    };
}