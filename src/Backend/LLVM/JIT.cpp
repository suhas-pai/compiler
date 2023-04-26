/*
 * Backend/LLVM/JIT.cpp
 */

#include "AST/Expr.h"
#include "AST/FunctionDecl.h"
#include "AST/FunctionProtoype.h"
#include "AST/ParamDecl.h"
#include "Backend/LLVM/JIT.h"
#include "Basic/SourceLocation.h"
#include "llvm/Support/Error.h"

namespace Backend::LLVM {
    JITHandler::JITHandler(std::unique_ptr<llvm::orc::ExecutionSession> ES,
                           const llvm::orc::JITTargetMachineBuilder JTMB,
                           const llvm::DataLayout DL) noexcept
    : Handler("JIT"), ES(std::move(ES)), DL(std::move(DL)),
      Mangle(*this->ES, this->DL),
      ObjectLayer(*this->ES,
                  []() noexcept {
                      return std::make_unique<llvm::SectionMemoryManager>();
                  }),
      CompileLayer(*this->ES, ObjectLayer,
                   std::make_unique<llvm::orc::ConcurrentIRCompiler>(
                       std::move(JTMB))),
      MainJD(this->ES->createBareJITDylib("<main>"))
    {
        MainJD.addGenerator(
            cantFail(
                llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
                    DL.getGlobalPrefix())));

        if (JTMB.getTargetTriple().isOSBinFormatCOFF()) {
            ObjectLayer.setOverrideObjectFlagsWithResponsibilityFlags(true);
            ObjectLayer.setAutoClaimResponsibilityForObjectSymbols(true);
        }

        // Open a new context and module.
        TheModule->setDataLayout(this->getDataLayout());

        // Create a new builder for the module.
        // Create a new pass manager attached to it.
    }

    auto JITHandler::Create() -> std::unique_ptr<JITHandler> {
        LLVMInitialize();

        auto EPC = llvm::orc::SelfExecutorProcessControl::Create();
        if (!EPC)
            return nullptr;

        auto ES =
            std::make_unique<llvm::orc::ExecutionSession>(std::move(*EPC));

        auto JTMB = llvm::orc::JITTargetMachineBuilder::detectHost();
        if (!JTMB) {
            return nullptr;
        }

        auto DL = JTMB->getDefaultDataLayoutForTarget();
        if (!DL) {
            return nullptr;
        }

        const auto Result =
            new JITHandler(std::move(ES), std::move(*JTMB), std::move(*DL));

        return std::unique_ptr<JITHandler>(Result);
    }

    JITHandler::~JITHandler() noexcept {
        if (auto Err = ES->endSession()) {
            ES->reportError(std::move(Err));
        }
    }

    void JITHandler::AllocCoreFields(const llvm::StringRef &Name) noexcept {
        Handler::AllocCoreFields(Name);
        TheModule->setDataLayout(this->getDataLayout());
    }

    void JITHandler::evalulateAndPrint(AST::Expr &Expr) noexcept {
        // JIT the module containing the anonymous expression, keeping a handle
        // so we can free it later.

        auto Proto =
            AST::FunctionPrototype(SourceLocation::invalid(),
                                   "__anon_expr",
                                   std::vector<AST::ParamDecl>());

        auto Func = AST::FunctionDecl(&Proto, &Expr);
        Func.codegen(*this);

        const auto RT = this->getMainJITDylib().createResourceTracker();
        auto TSM =
            llvm::orc::ThreadSafeModule(std::move(TheModule),
                                        std::move(TheContext));

        ExitOnErr(this->addModule(std::move(TSM), RT));
        Initialize("JIT");

        // Search the JIT for the __anon_expr symbol.
        const auto ExprSymbol = ExitOnErr(this->lookup("__anon_expr"));
        assert(ExprSymbol && "Function not found");

        // Get the symbol's address and cast it to the right type (takes no
        // arguments, returns a double) so we can call it as a native function.
        double (*FP)() = (double (*)())(intptr_t)ExprSymbol.getAddress();
        fprintf(stdout, "%f", FP());

        // Delete the anonymous expression module from the JIT.
        ExitOnErr(RT->remove());
    }
}