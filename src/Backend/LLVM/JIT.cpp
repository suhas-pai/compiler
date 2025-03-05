/*
 * Backend/LLVM/JIT.cpp
 */

#include "Backend/LLVM/Codegen.h"
#include "Backend/LLVM/Handler.h"
#include "Backend/LLVM/JIT.h"

#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"

#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/Passes/PassBuilder.h"

#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"

#include "Diag/Consumer.h"

namespace Backend::LLVM {
    static auto
    optimizeModule(llvm::orc::ThreadSafeModule TSM,
                   const llvm::orc::MaterializationResponsibility &R) noexcept
        -> llvm::Expected<llvm::orc::ThreadSafeModule>
    {
        TSM.withModuleDo([](llvm::Module &M) noexcept {
            auto FAM = llvm::FunctionAnalysisManager();
            auto PB = llvm::PassBuilder();

            PB.registerFunctionAnalyses(FAM);
            // Create a function pass manager.
            auto FPM = std::make_unique<llvm::FunctionPassManager>();

            // Add some optimizations.
            FPM->addPass(llvm::InstCombinePass());
            // Re-associate expressions.
            FPM->addPass(llvm::ReassociatePass());
            // Eliminate Common SubExpressions.
            FPM->addPass(llvm::GVNPass());
            // Simplify the control flow graph (deleting unreachable blocks, etc).
            FPM->addPass(llvm::SimplifyCFGPass());

            // Run the optimizations over all functions in the module being
            // added to the JIT.
            for (auto &F : M) {
                FPM->run(F, FAM);
            }
        });

        return std::move(TSM);
    }

    JITHandler::JITHandler(
        DiagnosticConsumer &Diag,
        const Parse::ParseUnit &Unit,
        std::unique_ptr<llvm::orc::ExecutionSession> ES,
        std::unique_ptr<llvm::orc::EPCIndirectionUtils> EPCIU,
        llvm::orc::JITTargetMachineBuilder JTMB,
        llvm::DataLayout DL) noexcept
      : Handler("jit", Diag),
        ES(std::move(ES)), EPCIU(std::move(EPCIU)), DL(std::move(DL)),
        Mangle(*this->ES, this->DL),
        ObjectLayer(*this->ES,
                    []() noexcept {
                        return std::make_unique<llvm::SectionMemoryManager>();
                    }),
        CompileLayer(*this->ES, ObjectLayer,
                     std::make_unique<llvm::orc::ConcurrentIRCompiler>(
                        std::move(JTMB))),
        OptimizeLayer(*this->ES, CompileLayer, optimizeModule),
        MainJD(this->ES->createBareJITDylib("<main>")), Unit(Unit)
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

        // FIXME:
    #if 0
        auto PowFunc =
            new AST::FunctionDecl(
                SourceLocation::invalid(),
                {
                    new AST::ParamVarDecl(
                        /*Name=*/"base",
                        SourceLocation::invalid(),
                        static_cast<AST::Expr *>(nullptr)),
                    new AST::ParamVarDecl(
                        /*Name=*/"exponent",
                        SourceLocation::invalid(),
                        static_cast<AST::Expr *>(nullptr))
                },
                Sema::BuiltinType::u64(),
                /*Body=*/nullptr,
                AST::ValueDecl::Linkage::External);

        Context.addDecl(PowFunc);
    #endif
        for (auto &[Name, Decl] : Unit.getTopLevelDeclList()) {
            if (llvm::isa<AST::LvalueNamedDecl>(Decl)) {
                addASTNode(Name, *Decl);
            }
        }
    }

    auto
    JITHandler::create(DiagnosticConsumer &Diag,
                       const Parse::ParseUnit &Unit) noexcept
        -> std::expected<std::unique_ptr<JITHandler>, llvm::Error>
    {
        initializeLLVM();

        auto EPC = llvm::orc::SelfExecutorProcessControl::Create();
        if (!EPC) {
            return std::unexpected(EPC.takeError());
        }

        auto ES =
            std::make_unique<llvm::orc::ExecutionSession>(std::move(*EPC));

        auto EPCIU = llvm::orc::EPCIndirectionUtils::Create(*ES);
        if (!EPCIU) {
            return std::unexpected(EPCIU.takeError());
        }

        const auto Lambda = []() noexcept {
            std::print("LazyCallThrough error: Could not find function body");
        };

        EPCIU->get()->createLazyCallThroughManager(
            *ES, llvm::orc::ExecutorAddr::fromPtr(&Lambda));

        if (auto Err = setUpInProcessLCTMReentryViaEPCIU(**EPCIU)) {
            return std::unexpected(std::move(Err));
        }

        auto JTMB =
            llvm::orc::JITTargetMachineBuilder(
                ES->getExecutorProcessControl().getTargetTriple());

        auto DL = JTMB.getDefaultDataLayoutForTarget();
        if (!DL) {
            return std::unexpected(DL.takeError());
        }

        auto Result =
            new JITHandler(Diag, Unit, std::move(ES), std::move(EPCIU.get()),
                           std::move(JTMB), std::move(*DL));

        return std::unique_ptr<JITHandler>(Result);
    }

    JITHandler::~JITHandler() noexcept {
        if (auto Err = this->ES->endSession()) {
            this->ES->reportError(std::move(Err));
        }

        if (auto Err = this->EPCIU->cleanup()) {
            this->ES->reportError(std::move(Err));
        }
    }

    void JITHandler::allocCoreFields(const llvm::StringRef &Name) noexcept {
        Handler::allocCoreFields(Name);
        this->getModule().setDataLayout(this->getDataLayout());
    }

    auto
    SetupGlobalDecls(JITHandler &Handler,
                     ::Backend::LLVM::ValueMap &ValueMap) noexcept -> bool
    {
        for (const auto &[Name, Decl] :
                Handler.getUnit().getTopLevelDeclList())
        {
            const auto ValDecl = llvm::dyn_cast<AST::LvalueNamedDecl>(Decl);
            if (ValDecl == nullptr) {
                continue;
            }

            // We need to be able to reference a function within its body,
            // so this case must be handled differently.

            if (const auto FuncDecl = llvm::dyn_cast<AST::FunctionDecl>(Decl)) {
                const auto FuncCodegenOpt =
                    FunctionDeclCodegen(*FuncDecl,
                                        Handler,
                                        Handler.getBuilder(),
                                        ValueMap);

                if (!FuncCodegenOpt.has_value()) {
                    return false;
                }

                ValueMap.setValue("FIXME_Name", FuncCodegenOpt.value());
                continue;
            }

            if (const auto VarDecl = llvm::dyn_cast<AST::VarDecl>(Decl)) {
                const auto CodegenOpt =
                    VarDeclCodegen(*VarDecl,
                                   Handler,
                                   Handler.getBuilder(),
                                   ValueMap);

                if (!CodegenOpt.has_value()) {
                    return false;
                }

                ValueMap.addValue(VarDecl->getName(), CodegenOpt.value());
                continue;
            }
        }

        return true;
    }

    auto
    SetupGlobalVarDeclAssigns(JITHandler &Handler,
                              ::Backend::LLVM::ValueMap &ValueMap,
                              llvm::IRBuilder<> &Builder) noexcept -> bool
    {
        for (const auto &[Name, Decl] :
                Handler.getUnit().getTopLevelDeclList())
        {
            if (const auto VarDecl = llvm::dyn_cast<AST::VarDecl>(Decl)) {
                if (!VarDecl->getQualifiers().isMutable()) {
                    continue;
                }

                auto AssignmentLhs =
                    AST::DeclRefExpr(VarDecl->getName(),
                                     /*NameLoc=*/SourceLocation::invalid());

                auto AssignmentOper =
                    AST::BinaryOperation(Parse::BinaryOperator::Assignment,
                                         /*Loc=*/SourceLocation::invalid(),
                                         AssignmentLhs,
                                         *VarDecl->getInitExpr());

                const auto BinOpCodegenOpt =
                    BinaryOperationCodegen(AssignmentOper,
                                           Handler,
                                           Builder,
                                           ValueMap);

                if (!BinOpCodegenOpt.has_value()) {
                    return false;
                }

                continue;
            }
        }

        return true;
    }

    bool
    JITHandler::evaluateAndPrint(AST::Stmt &Stmt,
                                 const bool PrintIR,
                                 const std::string_view Prefix,
                                 const std::string_view Suffix) noexcept
    {
        auto ValueMap = this->ValueMap;
        // FIXME:
        #if 0
        if (const auto FuncDecl = llvm::dyn_cast<AST::FunctionDecl>(&Stmt)) {
            if (getNameToASTNodeMap().contains(FuncDecl->getName())) {
                this->getDiag().emitError(FuncDecl->getNameLoc(),
                                          "\"" SV_FMT "\" is already defined",
                                          SV_FMT_ARG(FuncDecl->getName()));

                return false;
            }

            addASTNode(FuncDecl->getName(), *FuncDecl);
            if (PrintIR) {
                if (!SetupGlobalDecls(*this, ValueMap)) {
                    return false;
                }

                this->getModule().print(llvm::outs(), nullptr);

                const auto FuncValue = ValueMap.getValue(FuncDecl->getName());
                llvm::cast<llvm::Function>(FuncValue)->removeFromParent();
            }

            return true;
        }
        #endif

        auto StmtToExecute = &Stmt;
        if (const auto Decl = llvm::dyn_cast<AST::LvalueNamedDecl>(&Stmt)) {
            const std::string_view Name = Decl->getName();
            if (getNameToASTNodeMap().contains(Name)) {
                this->getDiag().consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = Decl->getNameLoc(),
                    .Message = std::format("\"{}\" is already defined", Name)
                });

                return false;
            }

            if (const auto VarDecl = llvm::dyn_cast<AST::VarDecl>(&Stmt)) {
                StmtToExecute =
                    new AST::DeclRefExpr(VarDecl->getName(),
                                         VarDecl->getNameLoc());
            }

            this->addASTNode(Name, *Decl);
        }

        if (!SetupGlobalDecls(*this, ValueMap)) {
            if (const auto Decl =
                    llvm::dyn_cast<AST::LvalueNamedDecl>(&Stmt))
            {
                this->removeASTNode(Decl->getName());
            }

            return false;
        }

        const auto Name = llvm::StringRef("__anon_expr");
        const auto ReturnStmt =
            std::unique_ptr<AST::ReturnStmt>(
                new AST::ReturnStmt(/*ReturnLoc=*/SourceLocation::invalid(),
                                    static_cast<AST::Expr *>(StmtToExecute)));

        auto FuncDecl =
            AST::FunctionDecl(/*Loc=*/SourceLocation::invalid(),
                              std::vector<AST::ParamVarDecl *>(),
                              /*ReturnTypeExpr=*/nullptr,
                              ReturnStmt.get());

        const auto FuncDeclCodegenOpt =
            FunctionDeclCodegen(FuncDecl, *this, this->getBuilder(), ValueMap);

        const auto BB =
            llvm::BasicBlock::Create(
                this->getContext(),
                "entry",
                llvm::cast<llvm::Function>(FuncDeclCodegenOpt.value()));

        auto BBBuilder = llvm::IRBuilder(BB);
        if (!SetupGlobalVarDeclAssigns(*this, ValueMap, BBBuilder)) {
            return false;
        }

        if (const auto Decl = llvm::dyn_cast<AST::LvalueNamedDecl>(&Stmt)) {
            this->addASTNode(Decl->getName(), *Decl);
        }

        if (PrintIR) {
            this->getModule().print(llvm::outs(), nullptr);
        }

        const auto RT = this->getMainJITDylib().createResourceTracker();
        auto TSM =
            llvm::orc::ThreadSafeModule(std::move(TheModule),
                                        std::move(TheContext));

        ExitOnErr(this->addModule(std::move(TSM), RT));
        initialize("JIT");

        // Search the JIT for the __anon_expr symbol.
        const auto ExprSymbol = ExitOnErr(this->lookup(Name));

        // Get the symbol's address and cast it to the right type (takes no
        // arguments, returns a double) so we can call it as a native function.
        double (*const FP)() = ExprSymbol.getAddress().toPtr<double (*)()>();
        std::print(stdout, "{}{}{}", Prefix, FP(), Suffix);

        // Delete the anonymous expression module from the JIT.
        ExitOnErr(RT->remove());
        return true;
    }
}
