/*
 * Backend/LLVM/JIT.cpp
 */

#include "AST/FunctionDecl.h"
#include "Backend/LLVM/JIT.h"

namespace Backend::LLVM {
    JITHandler::JITHandler(std::unique_ptr<llvm::orc::ExecutionSession> ES,
                           const llvm::orc::JITTargetMachineBuilder JTMB,
                           const llvm::DataLayout DL,
                           Interface::DiagnosticsEngine *const Diag) noexcept
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

        auto PowFuncProto =
            new AST::FunctionPrototype(
                SourceLocation::invalid(),
                "pow",
                {
                    AST::FunctionPrototype::ParamDecl(
                        SourceLocation::invalid(), /*Name=*/"base"),
                    AST::FunctionPrototype::ParamDecl(
                        SourceLocation::invalid(), /*Name=*/"exponent")
                });

        auto PowFunc =
            new AST::FunctionDecl(PowFuncProto,
                                  /*Body=*/nullptr,
                                  /*IsExternal=*/true);

        addASTNode(PowFuncProto->getName(), *PowFunc);
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

    auto
    SetupDecls(JITHandler &Handler,
               ::Backend::LLVM::ValueMap &ValueMap) noexcept -> bool
    {
        for (const auto Decl : Handler.getDeclList()) {
            // We need to be able to reference a function within its body,
            // so this case must be handled differently.

            if (const auto FuncDecl = llvm::dyn_cast<AST::FunctionDecl>(Decl)) {
                const auto Proto = FuncDecl->getPrototype();
                const auto ProtoCodegen =
                    Proto->codegen(Handler, Handler.getBuilder(), ValueMap);

                if (ProtoCodegen == nullptr) {
                    return false;
                }

                ValueMap.addValue(Proto->getName(), ProtoCodegen);
                const auto FinishedValue =
                    FuncDecl->finishPrototypeCodegen(Handler,
                                                     Handler.getBuilder(),
                                                     ValueMap,
                                                     ProtoCodegen);

                if (FinishedValue == nullptr) {
                    return false;
                }

                ValueMap.setValue(Proto->getName(), FinishedValue);
                continue;
            }

            if (const auto Value =
                    Decl->codegen(Handler, Handler.getBuilder(), ValueMap))
            {
                ValueMap.addValue(Decl->getName(), Value);
                continue;
            }

            return false;
        }

        return true;
    }

    auto
    UnsetupDecls(JITHandler &Handler,
                 ::Backend::LLVM::ValueMap &ValueMap) noexcept -> void
    {
        for (const auto Decl : Handler.getDeclList()) {
            if (const auto FuncDecl = llvm::dyn_cast<AST::FunctionDecl>(Decl)) {
                const auto Func = llvm::cast<llvm::Function>(
                    ValueMap.getValue(Decl->getName()));

                if (Func->getParent() != nullptr) {
                    Func->eraseFromParent();
                }
            }
        }
    }

    bool
    JITHandler::evalulateAndPrint(AST::Stmt &Stmt,
                                  const bool PrintIR,
                                  const std::string_view Prefix,
                                  const std::string_view Suffix) noexcept
    {
        // JIT the module containing the anonymous expression, keeping a handle
        // so we can free it later.

        auto ValueMap = ::Backend::LLVM::ValueMap();
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
            if (PrintIR) {
                if (!SetupDecls(*this, ValueMap)) {
                    return false;
                }

                getModule().print(llvm::outs(), nullptr);

                const auto FuncValue = ValueMap.getValue(Proto->getName());
                llvm::cast<llvm::Function>(FuncValue)->removeFromParent();

                UnsetupDecls(*this, ValueMap);
            }

            return true;
        }

        if (!SetupDecls(*this, ValueMap)) {
            return false;
        }

        const auto Name = llvm::StringRef("__anon_expr");
        auto Proto =
            AST::FunctionPrototype(
                SourceLocation::invalid(),
                /*Name=*/Name,
                std::vector<AST::FunctionPrototype::ParamDecl>());

        auto Func = AST::FunctionDecl(&Proto, static_cast<AST::Expr *>(&Stmt));
        if (Func.codegen(*this, getBuilder(), ValueMap) == nullptr) {
            return false;
        }

        if (const auto Decl = llvm::dyn_cast<AST::Decl>(&Stmt)) {
            addASTNode(Decl->getName(), *Decl);
        }

        if (PrintIR) {
            getModule().print(llvm::outs(), nullptr);
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
        UnsetupDecls(*this, ValueMap);

        return true;
    }
}