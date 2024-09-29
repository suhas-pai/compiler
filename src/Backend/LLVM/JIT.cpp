/*
 * Backend/LLVM/JIT.cpp
 */

#include "AST/FunctionDecl.h"
#include "AST/ReturnStmt.h"
#include "AST/VarDecl.h"

#include "Backend/LLVM/Codegen.h"
#include "Backend/LLVM/Handler.h"
#include "Backend/LLVM/JIT.h"

#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"

#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

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
                                  AST::Decl::Linkage::External);

        Context.addDecl(PowFunc);
        const auto &DeclMap =
            Context.getSymbolTable().getGlobalScope().getDeclMap();

        for (auto &[Name, Decl] : DeclMap) {
            if (Decl->getLinkage() == AST::Decl::Linkage::External) {
                const auto ValueOpt = codegen(*Decl, getBuilder(), ValueMap);
                if (!ValueOpt.has_value()) {
                    getDiag().emitInternalError(
                        "Failed to codegen for symbol " SV_FMT,
                        SV_FMT_ARG(Decl->getName()));
                    continue;
                }

                ValueMap.addValue(Decl->getName(), ValueOpt.value());
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
            if (Decl->getLinkage() == AST::Decl::Linkage::External) {
                continue;
            }

            // We need to be able to reference a function within its body,
            // so this case must be handled differently.

            if (const auto FuncDecl = llvm::dyn_cast<AST::FunctionDecl>(Decl)) {
                auto &Proto = FuncDecl->getPrototype();
                const auto ProtoCodegenOpt =
                    FunctionPrototypeCodegen(Proto,
                                             Handler,
                                             Handler.getBuilder(),
                                             ValueMap);

                if (!ProtoCodegenOpt.has_value()) {
                    return false;
                }

                const auto ProtoCodegen = ProtoCodegenOpt.value();
                ValueMap.addValue(Proto.getName(), ProtoCodegen);

                const auto FinishedValueOpt =
                    FunctionDeclFinishPrototypeCodegen(*FuncDecl,
                                                       Handler,
                                                       Handler.getBuilder(),
                                                       ValueMap,
                                                       ProtoCodegen,
                                                       /*BB=*/nullptr);

                if (!FinishedValueOpt.has_value()) {
                    return false;
                }

                ValueMap.setValue(Proto.getName(), FinishedValueOpt.value());
                continue;
            }

            if (const auto VarDecl = llvm::dyn_cast<AST::VarDecl>(Decl)) {
                VarDecl->setIsGlobal(true);
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
                    AST::VariableRef(/*NameLoc=*/SourceLocation::invalid(),
                                     VarDecl->getName());

                auto AssignmentOper =
                    AST::BinaryOperation(/*Loc=*/SourceLocation::invalid(),
                                         Parse::BinaryOperator::Assignment,
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
        if (const auto FuncDecl = llvm::dyn_cast<AST::FunctionDecl>(&Stmt)) {
            const auto Proto = FuncDecl->getPrototype();
            if (getNameToASTNodeMap().contains(Proto.getName())) {
                getDiag().emitError(Proto.getNameLoc(),
                                    "\"" SV_FMT "\" is already defined",
                                    SV_FMT_ARG(Proto.getName()));

                return false;
            }

            addASTNode(Proto.getName(), *FuncDecl);
            if (PrintIR) {
                if (!SetupGlobalDecls(*this, ValueMap)) {
                    return false;
                }

                getModule().print(llvm::outs(), nullptr);

                const auto FuncValue = ValueMap.getValue(Proto.getName());
                llvm::cast<llvm::Function>(FuncValue)->removeFromParent();
            }

            return true;
        }

        auto StmtToExecute = &Stmt;
        if (const auto Decl = llvm::dyn_cast<AST::Decl>(&Stmt)) {
            const std::string_view Name = Decl->getName();
            if (getNameToASTNodeMap().contains(Name)) {
                getDiag().emitError(Decl->getLoc(),
                                    "\"" SV_FMT "\" is already defined",
                                    SV_FMT_ARG(Name));

                return false;
            }

            if (const auto VarDecl = llvm::dyn_cast<AST::VarDecl>(&Stmt)) {
                StmtToExecute =
                    new AST::VariableRef(Decl->getLoc(), VarDecl->getName());
            }

            addASTNode(Name, *Decl);
        }

        if (!SetupGlobalDecls(*this, ValueMap)) {
            if (const auto Decl = llvm::dyn_cast<AST::Decl>(&Stmt)) {
                removeASTNode(Decl->getName());
            }

            return false;
        }

        const auto Name = llvm::StringRef("__anon_expr");
        auto Proto =
            AST::FunctionPrototype(
                /*NameLoc=*/SourceLocation::invalid(),
                Name,
                std::vector<AST::FunctionPrototype::ParamDecl>());

        const auto ReturnStmt =
            std::unique_ptr<AST::ReturnStmt>(
                new AST::ReturnStmt(
                    /*ReturnLoc=*/SourceLocation::invalid(),
                    static_cast<AST::Expr *>(StmtToExecute)));

        auto FuncDecl =
            AST::FunctionDecl(&Proto,
                              ReturnStmt.get(),
                              AST::Decl::Linkage::Private);

        const auto ProtoCodegenOpt =
            FunctionPrototypeCodegen(FuncDecl.getPrototype(),
                                     *this,
                                     getBuilder(),
                                     ValueMap);

        if (!ProtoCodegenOpt.has_value()) {
            return false;
        }

        const auto BB =
            llvm::BasicBlock::Create(
                getContext(),
                "entry",
                llvm::cast<llvm::Function>(ProtoCodegenOpt.value()));

        auto BBBuilder = llvm::IRBuilder(BB);
        if (!SetupGlobalVarDeclAssigns(*this, ValueMap, BBBuilder)) {
            return false;
        }

        const auto ProtoCodegen = ProtoCodegenOpt.value();
        const auto FuncProtoValueOpt =
            FunctionDeclFinishPrototypeCodegen(FuncDecl,
                                               *this,
                                               *Builder,
                                               ValueMap,
                                               ProtoCodegen,
                                               BB);

        if (!FuncProtoValueOpt.has_value()) {
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