/*
 * Backend/LLVM/JIT.cpp
 */

#include "Backend/LLVM/Codegen.h"
#include "Backend/LLVM/Handler.h"
#include "Backend/LLVM/JIT.h"

#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"

#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

#include "Sema/Types/Builtin.h"

namespace Backend::LLVM {
    JITHandler::JITHandler(std::unique_ptr<llvm::orc::ExecutionSession> ES,
                           const llvm::orc::JITTargetMachineBuilder JTMB,
                           const llvm::DataLayout DL,
                           AST::Context &Context,
                           Interface::DiagnosticsEngine &Diag) noexcept
    : Handler("JIT", Diag), ES(std::move(ES)), DL(std::move(DL)),
      Mangle(*this->ES, this->DL),
      ObjectLayer(*this->ES,
                  []() noexcept {
                      return std::make_unique<llvm::SectionMemoryManager>();
                  }),
      CompileLayer(*this->ES, ObjectLayer,
                   std::make_unique<llvm::orc::ConcurrentIRCompiler>(
                       std::move(JTMB))),
      MainJD(this->ES->createBareJITDylib("<main>")),
      Context(Context)
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
                        static_cast<AST::TypeRef *>(nullptr)),
                    new AST::ParamVarDecl(
                        /*Name=*/"exponent",
                        SourceLocation::invalid(),
                        static_cast<AST::TypeRef *>(nullptr))
                },
                Sema::BuiltinType::u64(),
                /*Body=*/nullptr,
                AST::ValueDecl::Linkage::External);

        Context.addDecl(PowFunc);
    #endif
        const auto &DeclMap =
            Context.getSymbolTable().getGlobalScope().getDeclMap();

        for (auto &[Name, Decl] : DeclMap) {
            auto ValDecl = llvm::dyn_cast<AST::LvalueNamedDecl>(Decl);
            if (ValDecl == nullptr) {
                continue;
            }

            addASTNode(Name, *Decl);
        }
    }

    auto
    JITHandler::create(Interface::DiagnosticsEngine &Diag,
                       AST::Context &Context) noexcept
        -> std::unique_ptr<JITHandler>
    {
        initializeLLVM();

        auto EPC = llvm::orc::SelfExecutorProcessControl::Create();
        if (!EPC) {
            return nullptr;
        }

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
                           Context,
                           Diag);

        return std::unique_ptr<JITHandler>(Result);
    }

    JITHandler::~JITHandler() noexcept {
        if (auto Err = ES->endSession()) {
            ES->reportError(std::move(Err));
        }
    }

    void JITHandler::allocCoreFields(const llvm::StringRef &Name) noexcept {
        Handler::allocCoreFields(Name);
        getModule().setDataLayout(this->getDataLayout());
    }

    auto
    SetupGlobalDecls(JITHandler &Handler,
                     ::Backend::LLVM::ValueMap &ValueMap) noexcept -> bool
    {
        const auto &DeclMap =
            Handler
                .getASTContext()
                .getSymbolTable()
                .getGlobalScope()
                .getDeclMap();

        for (const auto &[Name, Decl] : DeclMap) {
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
        const auto &DeclMap =
            Handler
                .getASTContext()
                .getSymbolTable()
                .getGlobalScope()
                .getDeclMap();

        for (const auto &[Name, Decl] : DeclMap) {
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
                                         &AssignmentLhs,
                                         VarDecl->getInitExpr());

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
    JITHandler::evalulateAndPrint(AST::Stmt &Stmt,
                                  const bool PrintIR,
                                  const std::string_view Prefix,
                                  const std::string_view Suffix) noexcept
    {
        auto ValueMap = this->ValueMap;
        // FIXME:
        #if 0
        if (const auto FuncDecl = llvm::dyn_cast<AST::FunctionDecl>(&Stmt)) {
            if (getNameToASTNodeMap().contains(FuncDecl->getName())) {
                getDiag().emitError(FuncDecl->getNameLoc(),
                                    "\"" SV_FMT "\" is already defined",
                                    SV_FMT_ARG(FuncDecl->getName()));

                return false;
            }

            addASTNode(FuncDecl->getName(), *FuncDecl);
            if (PrintIR) {
                if (!SetupGlobalDecls(*this, ValueMap)) {
                    return false;
                }

                getModule().print(llvm::outs(), nullptr);

                const auto FuncValue = ValueMap.getValue(FuncDecl->getName());
                llvm::cast<llvm::Function>(FuncValue)->removeFromParent();
            }

            return true;
        }
        #endif

        auto StmtToExecute = &Stmt;
        if (const auto Decl = llvm::dyn_cast<AST::NamedDecl>(&Stmt)) {
            const std::string_view Name = Decl->getName();
            if (getNameToASTNodeMap().contains(Name)) {
                getDiag().emitError(Decl->getLoc(),
                                    "\"" SV_FMT "\" is already defined",
                                    SV_FMT_ARG(Name));

                return false;
            }

            if (const auto VarDecl = llvm::dyn_cast<AST::VarDecl>(&Stmt)) {
                StmtToExecute =
                    new AST::DeclRefExpr(VarDecl->getName(), Decl->getLoc());
            }

            addASTNode(Name, *Decl);
        }

        if (!SetupGlobalDecls(*this, ValueMap)) {
            if (const auto Decl = llvm::dyn_cast<AST::NamedDecl>(&Stmt)) {
                removeASTNode(Decl->getName());
            }

            return false;
        }

        const auto Name = llvm::StringRef("__anon_expr");
        const auto ReturnStmt =
            std::unique_ptr<AST::ReturnStmt>(
                new AST::ReturnStmt(
                    /*ReturnLoc=*/SourceLocation::invalid(),
                    static_cast<AST::Expr *>(StmtToExecute)));

        auto FuncDecl =
            AST::FunctionDecl(
                /*Loc=*/SourceLocation::invalid(),
                std::vector<AST::ParamVarDecl *>(),
                &Sema::BuiltinType::voidType(),
                ReturnStmt.get(),
                AST::Linkage::Private);

        const auto FuncDeclCodegenOpt =
            FunctionDeclCodegen(FuncDecl,
                                *this,
                                getBuilder(),
                                ValueMap);

        const auto BB =
            llvm::BasicBlock::Create(
                getContext(),
                "entry",
                llvm::cast<llvm::Function>(FuncDeclCodegenOpt.value()));

        auto BBBuilder = llvm::IRBuilder(BB);
        if (!SetupGlobalVarDeclAssigns(*this, ValueMap, BBBuilder)) {
            return false;
        }

        if (const auto Decl = llvm::dyn_cast<AST::NamedDecl>(&Stmt)) {
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
        initialize("JIT");

        // Search the JIT for the __anon_expr symbol.
        const auto ExprSymbol = ExitOnErr(this->lookup(Name));

        // Get the symbol's address and cast it to the right type (takes no
        // arguments, returns a double) so we can call it as a native function.
        double (*const FP)() = ExprSymbol.getAddress().toPtr<double (*)()>();
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
