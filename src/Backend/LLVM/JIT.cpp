/*
 * Backend/LLVM/JIT.cpp
 */

#include "AST/FunctionDecl.h"
#include "Backend/LLVM/Handler.h"
#include "Backend/LLVM/JIT.h"

namespace Backend::LLVM {
    JITHandler::JITHandler(std::unique_ptr<llvm::orc::ExecutionSession> ES,
                           const llvm::orc::JITTargetMachineBuilder JTMB,
                           const llvm::DataLayout DL,
                           Interface::DiagnosticsEngine *Diag) noexcept
    : Handler("JIT", Diag), ES(std::move(ES)), DL(std::move(DL)),
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

    auto JITHandler::Create(Interface::DiagnosticsEngine *Diag)
        -> std::unique_ptr<JITHandler>
    {
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
            new JITHandler(std::move(ES),
                           std::move(*JTMB),
                           std::move(*DL),
                           Diag);

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

    bool
    JITHandler::evalulateAndPrint(AST::Stmt &Stmt,
                                  const std::string_view Prefix,
                                  const std::string_view Suffix) noexcept
    {
        // JIT the module containing the anonymous expression, keeping a handle
        // so we can free it later.

        if (const auto FuncDecl = llvm::dyn_cast<AST::FunctionDecl>(&Stmt)) {
            const auto Proto = FuncDecl->getPrototype();
            if (getNameToASTNodeMap().contains(Proto->getName())) {
                if (const auto Diag = getDiag()) {
                    Diag->emitError("\"" SV_FMT "\" is already defined",
                                    SV_FMT_ARG(Proto->getName()));
                }

                return false;
            }

            addASTNode(Proto->getName(), *FuncDecl);
            return true;
        } else if (const auto Decl = llvm::dyn_cast<AST::Decl>(&Stmt)) {
            addASTNode(Decl->getName(), *Decl);
        }

        auto ValueMap = ::Backend::LLVM::ValueMap();
        for (const auto Decl : getDeclList()) {
            if (const auto Value = Decl->codegen(*this, ValueMap)) {
                ValueMap.addValue(Decl->getName(), Value);
                continue;
            }

            return false;
        }

        const auto Name = llvm::StringRef("__anon_expr");
        auto Proto =
            AST::FunctionPrototype(
                SourceLocation::invalid(),
                /*Name=*/Name,
                std::vector<AST::FunctionPrototype::ParamDecl>());

        auto Func = AST::FunctionDecl(&Proto, static_cast<AST::Expr *>(&Stmt));
        if (Func.codegen(*this, ValueMap) == nullptr) {
            return false;
        }

        const auto RT = this->getMainJITDylib().createResourceTracker();
        auto TSM =
            llvm::orc::ThreadSafeModule(std::move(TheModule),
                                        std::move(TheContext));

        ExitOnErr(this->addModule(std::move(TSM), RT));
        Initialize("JIT");

        // Search the JIT for the __anon_expr symbol.
        const auto ExprSymbol = ExitOnErr(this->lookup(Name));
        assert(ExprSymbol && "Function not found");

        // Get the symbol's address and cast it to the right type (takes no
        // arguments, returns a double) so we can call it as a native function.
        double (*FP)() = (double (*)())(intptr_t)ExprSymbol.getAddress();
        fprintf(stdout,
                SV_FMT "%f" SV_FMT,
                SV_FMT_ARG(Prefix),
                FP(),
                SV_FMT_ARG(Suffix));

        // Delete the anonymous expression module from the JIT.
        ExitOnErr(RT->remove());
        return true;
    }
}