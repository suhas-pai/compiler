/*
 * Backend/LLVM/Codegen.cpp
 */

#include "Backend/LLVM/Codegen.h"
#include "llvm/IR/Verifier.h"

namespace Backend::LLVM {
    auto
    BinaryOperationCodegen(AST::BinaryOperation &BinOp,
                           Handler &Handler,
                           llvm::IRBuilder<> &Builder,
                           ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        const auto LeftOpt =
            Handler.codegen(BinOp.getLhs(), Builder, ValueMap);

        if (!LeftOpt.has_value()) {
            return std::nullopt;
        }

        const auto RightOpt =
            Handler.codegen(BinOp.getRhs(), Builder, ValueMap);

        if (!RightOpt.has_value()) {
            return std::nullopt;
        }

        auto &Context = Handler.getContext();

        const auto Left = LeftOpt.value();
        const auto Right = RightOpt.value();

        switch (BinOp.getOperator()) {
            case Parse::BinaryOperator::Assignment:
                __builtin_unreachable();
            case Parse::BinaryOperator::Add:
                return Builder.CreateFAdd(Left, Right, /*Name=*/"addtmp");
            case Parse::BinaryOperator::Subtract:
                return Builder.CreateFSub(Left, Right, /*Name=*/"subtmp");
            case Parse::BinaryOperator::Multiply:
                return Builder.CreateFMul(Left, Right, /*Name=*/"multmp");
            case Parse::BinaryOperator::Modulo:
                return Builder.CreateFRem(Left, Right, /*Name=*/"modtmp");
            case Parse::BinaryOperator::Divide:
                return Builder.CreateFDiv(Left, Right, /*Name=*/"divtmp");
            case Parse::BinaryOperator::LogicalAnd:
                return Builder.CreateAnd(Left, Right, /*Name=*/"andtmp");
            case Parse::BinaryOperator::LogicalOr:
                return Builder.CreateOr(Left, Right, /*Name=*/"ortmp");
            case Parse::BinaryOperator::BitwiseAnd:
                return Builder.CreateAnd(Left, Right, /*Name=*/"andtmp");
            case Parse::BinaryOperator::BitwiseOr:
                return Builder.CreateOr(Left, Right, /*Name=*/"ortmp");
            case Parse::BinaryOperator::BitwiseXor:
                return Builder.CreateXor(Left, Right, /*Name=*/"xortmp");
            case Parse::BinaryOperator::LeftShift:
                return Builder.CreateShl(Left, Right, /*Name=*/"lshifttmp");
            case Parse::BinaryOperator::RightShift:
                return Builder.CreateAShr(Left, Right, /*Name=*/"rshifttmp");
            case Parse::BinaryOperator::LessThan:
                return Builder.CreateFCmpOLT(Left, Right, /*Name=*/"lttmp");
            case Parse::BinaryOperator::GreaterThan:
                return Builder.CreateFCmpOGT(Left, Right, /*Name=*/"gttmp");
            case Parse::BinaryOperator::LessThanOrEqual:
                return Builder.CreateFCmpOLE(Left, Right, /*Name=*/"letmp");
            case Parse::BinaryOperator::GreaterThanOrEqual:
                return Builder.CreateFCmpOGE(Left, Right, /*Name=*/"getmp");
            case Parse::BinaryOperator::Equality:
                return Builder.CreateFCmpOEQ(Left, Right, /*Name=*/"eqtmp");
            case Parse::BinaryOperator::Inequality:
                return Builder.CreateFCmpONE(Left, Right, /*Name=*/"nqtmp");
            case Parse::BinaryOperator::Power: {
                const auto LeftDouble =
                    Builder.CreateUIToFP(Left,
                                         llvm::Type::getDoubleTy(Context));
                const auto RightDouble =
                    Builder.CreateUIToFP(Right,
                                         llvm::Type::getDoubleTy(Context));

                const auto PowFunc =
                    llvm::dyn_cast_if_present<llvm::Function>(
                        ValueMap.getValue("pow"));

                if (PowFunc == nullptr) {
                    Handler.getDiag().consume({
                        .Level = DiagnosticLevel::Error,
                        .Location = BinOp.getLoc(),
                        .Message = "** operator only supported on JIT"
                    });

                    return std::nullopt;
                }

                return Builder.CreateCall(PowFunc, {LeftDouble, RightDouble},
                                          "powtmp");
            }
            case Parse::BinaryOperator::AddAssign:
            case Parse::BinaryOperator::SubtractAssign:
            case Parse::BinaryOperator::MultiplyAssign:
            case Parse::BinaryOperator::DivideAssign:
            case Parse::BinaryOperator::ModuloAssign:
            case Parse::BinaryOperator::BitwiseAndAssign:
            case Parse::BinaryOperator::BitwiseOrAssign:
            case Parse::BinaryOperator::BitwiseXorAssign:
            case Parse::BinaryOperator::LeftShiftAssign:
            case Parse::BinaryOperator::RightShiftAssign:
            case Parse::BinaryOperator::As:
                __builtin_unreachable();
        }

        __builtin_unreachable();
    }

    auto
    CharLiteralCodegen(AST::CharLiteral &CharLit,
                       Handler &Handler,
                       llvm::IRBuilder<> &Builder,
                       ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        auto &Context = Handler.getContext();
        return llvm::ConstantInt::get(llvm::Type::getInt8Ty(Context),
                                      CharLit.getValue(),
                                      /*IsSigned=*/false);
    }

    auto
    CompoundStmtCodegen(AST::CompoundStmt &CompoundStmt,
                        Handler &Handler,
                        llvm::IRBuilder<> &Builder,
                        ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        // Create blocks for the then and else cases. Insert the 'then' block at
        // the end of the function.

        auto AddedDecls = std::vector<std::string_view>();
        for (const auto &Stmt : CompoundStmt.getStmtList()) {
            if (const auto ResultOpt =
                    Handler.codegen(*Stmt, Builder, ValueMap))
            {
                if (const auto Decl =
                        llvm::dyn_cast<AST::LvalueNamedDecl>(Stmt))
                {
                    ValueMap.add(Decl->getName(), ResultOpt.value());
                    AddedDecls.push_back(Decl->getName());
                }

                continue;
            }

            for (const auto Name : AddedDecls) {
                ValueMap.removeValue(Name);
            }

            return std::nullopt;
        }

        for (const auto Name : AddedDecls) {
            ValueMap.removeValue(Name);
        }

        return nullptr;
    }

    auto
    CallExprCodegen(AST::CallExpr &FuncCall,
                    Handler &Handler,
                    llvm::IRBuilder<> &Builder,
                    ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        auto Callee = FuncCall.getCalleeExpr();
        const auto CalleeCodegen = Handler.codegen(*Callee, Builder, ValueMap);

        if (!CalleeCodegen.has_value()) {
            return std::nullopt;
        }

        const auto FuncDecl = llvm::dyn_cast<AST::FunctionDecl>(Callee);
        if (FuncDecl == nullptr) {
            Handler.getDiag().consume({
                .Level = DiagnosticLevel::Error,
                .Location = FuncCall.getParenLoc(),
                .Message = "Attempting to call a non-function statement"
            });

            return std::nullopt;
        }

        // FIXME:
        #if 0
        const auto Node = Handler.getASTNode("func_");
        DIAG_ASSERT(Handler.getDiag(),
                    Node != nullptr,
                    "Function \"" SV_FMT "\" is not in AST, but is in "
                    "ValueMap",
                    SV_FMT_ARG(FuncCall.getName()));

        const auto FuncLLVM = llvm::cast<llvm::Function>(FuncValue);
        if (FuncCall.getArgs().size() != FuncDecl->getParamList().size()) {
            Handler.getDiag().emitError(
                FuncCall.getParenLoc(),
                "%" PRIdPTR " arguments provided to function \"" SV_FMT "\", "
                "expected %" PRIdPTR,
                FuncCall.getArgs().size(),
                SV_FMT_ARG(FuncCall.getName()),
                FuncDecl->getParamList().size());

            return std::nullopt;
        }

        auto ArgsV = std::vector<llvm::Value *>();
        ArgsV.reserve(FuncCall.getArgs().size());

        for (auto I = unsigned(); I != FuncCall.getArgs().size(); ++I) {
            if (const auto ResultOpt =
                    Handler.codegen(*FuncCall.getArgs()[I], Builder, ValueMap))
            {
                ArgsV.push_back(ResultOpt.value());
                continue;
            }

            return std::nullopt;
        }

        return Builder.CreateCall(FuncLLVM, ArgsV, "calltmp");
    #endif
        return std::nullopt;
    }

    auto
    FunctionDeclCodegen(AST::FunctionDecl &FuncDecl,
                        Handler &Handler,
                        llvm::IRBuilder<> &Builder,
                        ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        auto &Context = Handler.getContext();
        auto &Module = Handler.getModule();

        const auto DoubleTy = llvm::Type::getDoubleTy(Context);
        const auto ParamList =
            std::vector(FuncDecl.getParamList().size(), DoubleTy);
        const auto FT =
            llvm::FunctionType::get(DoubleTy, ParamList,/*isVarArg=*/false);
        const auto FuncDeclCodegen =
            llvm::Function::Create(FT,
                                   llvm::Function::ExternalLinkage,
                                   "FIXME_Name",
                                   Module);

        // Set names for all arguments.
        auto Idx = unsigned();
        for (auto &Arg : FuncDeclCodegen->args()) {
            Arg.setName(FuncDecl.getParamList()[Idx++]->getName());
        }

        // Avoid adding the function to the symbol table if it is external.
        const auto Body = FuncDecl.getBody();
        if (Body == nullptr) {
            return FuncDeclCodegen;
        }

        // FIXME:
        // ValueMap.addValue(FuncDecl.getName(), FuncDeclCodegen);

        const auto Function = llvm::cast<llvm::Function>(FuncDeclCodegen);
        const auto BB =
            llvm::BasicBlock::Create(Handler.getContext(),
                                     "entry",
                                     Function);

        for (auto &Arg : Function->args()) {
            ValueMap.add(Arg.getName(), &Arg);
        }

        auto BodyIRBuilder = llvm::IRBuilder(BB);
        const auto BodyValOpt = Handler.codegen(*Body, BodyIRBuilder, ValueMap);

        if (!BodyValOpt.has_value()) {
            Function->removeFromParent();
            return std::nullopt;
        }

        for (auto &Arg : Function->args()) {
            ValueMap.removeValue(Arg.getName());
        }

        // Finish off the function.
        // Validate the generated code, checking for consistency.
        llvm::verifyFunction(*Function);

        // Run the optimizer on the function.
        Handler.getFPM().run(*Function, Handler.getFAM());
        return Function;
    }

    auto
    IfStmtCodegen(AST::IfExpr &IfStmt,
                  Handler &Handler,
                  llvm::IRBuilder<> &Builder,
                  ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        const auto CondValueOpt =
            Handler.codegen(*IfStmt.getCond(), Builder, ValueMap);

        if (!CondValueOpt.has_value()) {
            return std::nullopt;
        }

        auto CondValue = CondValueOpt.value();
        auto &Context = Handler.getContext();

        if (!llvm::isa<llvm::ConstantInt>(CondValue)) {
            // Convert condition to a bool by comparing non-equal to 0.0.
            const auto ZeroFP =
                llvm::ConstantFP::get(Context, llvm::APFloat(0.0));

            CondValue = Builder.CreateICmpNE(CondValue, ZeroFP, "ifcond");
        } else {
            // Convert condition to a bool by comparing non-equal to 0.0.
            const auto ZeroFP =
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 0);

            CondValue = Builder.CreateICmpNE(CondValue, ZeroFP, "ifcond");
        }

        const auto TheFunction = Builder.GetInsertBlock()->getParent();

        // Create blocks for the then and else cases.  Insert the 'then' block
        // at the end of the function.

        auto ThenBB = llvm::BasicBlock::Create(Context, "then", TheFunction);
        auto ElseBB = llvm::BasicBlock::Create(Context, "else");

        // Emit then value.
        const auto CondBrValue =
            Builder.CreateCondBr(CondValue, ThenBB, ElseBB);

        auto ThenIRBuilder = llvm::IRBuilder(ThenBB);

        const auto Then = IfStmt.getThen();
        const auto Else = IfStmt.getElse();

        if (!Handler.codegen(*Then, ThenIRBuilder, ValueMap).has_value()) {
            ThenBB->eraseFromParent();
            ElseBB->eraseFromParent();

            CondBrValue->eraseFromParent();
            CondValue->deleteValue();

            return std::nullopt;
        }

        // Emit else block.
        TheFunction->insert(TheFunction->end(), ElseBB);
        if (Else != nullptr) {
            auto ElseIRBuilder = llvm::IRBuilder(ElseBB);
            const auto ElseValueOpt =
                Handler.codegen(*Else, ElseIRBuilder, ValueMap);

            if (!ElseValueOpt.has_value()) {
                ThenBB->eraseFromParent();
                ElseBB->eraseFromParent();

                CondBrValue->eraseFromParent();
                CondValue->deleteValue();

                return std::nullopt;
            }
        }

        return TheFunction;
    }

    auto
    NumberLiteralCodegen(AST::NumberLiteral &NumLit,
                         Handler &Handler,
                         llvm::IRBuilder<> &Builder,
                         ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        auto &Context = Handler.getContext();
        #if 0
        return llvm::ConstantInt::get(llvm::Type::getInt64Ty(Context),
                                      Number.UInt,
                                      /*IsSigned=*/true);
        #endif

        const auto FloatVal = (double)NumLit.getNumber().Success.UInt;
        return llvm::ConstantFP::get(Context, llvm::APFloat(FloatVal));
    }

    auto
    ParenExprCodegen(AST::ParenExpr &ParenExpr,
                     Handler &Handler,
                     llvm::IRBuilder<> &Builder,
                     ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        return Handler.codegen(*ParenExpr.getChildExpr(), Builder, ValueMap);
    }

    auto
    ReturnStmtCodegen(AST::ReturnStmt &RetStmt,
                      Handler &Handler,
                      llvm::IRBuilder<> &Builder,
                      ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        if (RetStmt.getValue() == nullptr) {
            return Builder.CreateRetVoid();
        }

        if (const auto ResultOpt =
                Handler.codegen(*RetStmt.getValue(), Builder, ValueMap))
        {
            return Builder.CreateRet(ResultOpt.value());
        }

        return std::nullopt;
    }

    auto
    StringLiteralCodegen(AST::StringLiteral &StrLit,
                         Handler &Handler,
                         llvm::IRBuilder<> &Builder,
                         ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        return llvm::ConstantDataArray::getString(Handler.getContext(),
                                                  StrLit.value());
    }

    auto
    UnaryOperationCodegen(AST::UnaryOperation &UnaryOp,
                          Handler &Handler,
                          llvm::IRBuilder<> &Builder,
                          ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        const auto OperandOpt =
            Handler.codegen(UnaryOp.getOperand(), Builder, ValueMap);

        if (!OperandOpt.has_value()) {
            return std::nullopt;
        }

        const auto Operand = OperandOpt.value();
        switch (UnaryOp.getOperator()) {
            case Parse::UnaryOperator::Negate:
                return Builder.CreateNeg(Operand);
            case Parse::UnaryOperator::LogicalNot:
                assert(false && "Logical-Not not yet supported");
            case Parse::UnaryOperator::BitwiseNot:
                return Builder.CreateNot(Operand);
            case Parse::UnaryOperator::Increment: {
                const auto Value =
                    llvm::ConstantInt::get(Operand->getType(), 1);
                return Builder.CreateAdd(Operand, Value);
            }
            case Parse::UnaryOperator::Decrement: {
                const auto Value =
                    llvm::ConstantInt::get(Operand->getType(), 1);

                return Builder.CreateSub(Operand, Value);
            }
            case Parse::UnaryOperator::AddressOf:
                Handler.getDiag().consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = UnaryOp.getLoc(),
                    .Message = "AddressOf not yet supported"
                });

                return std::nullopt;
            case Parse::UnaryOperator::Spread:
            case Parse::UnaryOperator::Optional:
            case Parse::UnaryOperator::Pointer:
                __builtin_unreachable();
              break;
        }

        __builtin_unreachable();
    }

    auto
    VarDeclCodegen(AST::VarDecl &VarDecl,
                   Handler &Handler,
                   llvm::IRBuilder<> &Builder,
                   ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        if (ValueMap.getValue(VarDecl.getName()) != nullptr) {
            Handler.getDiag().consume({
                .Level = DiagnosticLevel::Error,
                .Location = VarDecl.getNameLoc(),
                .Message =
                    std::format("Variable \'{}\' is already defined",
                                VarDecl.getName())
            });

            return std::nullopt;
        }

        if (VarDecl.getInitExpr() == nullptr) {
            const auto Type = llvm::Type::getDoubleTy(Handler.getContext());
            const auto GV =
                new llvm::GlobalVariable(
                    Handler.getModule(),
                    Type,
                    /*isConstant=*/!VarDecl.getQualifiers().isMutable(),
                    llvm::GlobalVariable::LinkageTypes::ExternalLinkage,
                    /*Initializer=*/nullptr,
                    VarDecl.getName());

            return GV;
        }

        const auto ResultOpt =
            Handler.codegen(*VarDecl.getInitExpr(), Builder, ValueMap);

        if (!ResultOpt.has_value()) {
            return std::nullopt;
        }

        const auto FuncParent = Builder.GetInsertBlock()->getParent();
        auto TmpB =
            llvm::IRBuilder(&FuncParent->getEntryBlock(),
                            FuncParent->getEntryBlock().begin());

        const auto DoubleTy = llvm::Type::getDoubleTy(Handler.getContext());
        const auto AllocaBlock =
            TmpB.CreateAlloca(DoubleTy, nullptr, VarDecl.getName());

        Builder.CreateStore(ResultOpt.value(), AllocaBlock);
        return AllocaBlock;
    }

    auto
    DeclRefExprCodegen(AST::DeclRefExpr &DeclRef,
                       Handler &Handler,
                       llvm::IRBuilder<> &Builder,
                       ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        if (const auto Value = ValueMap.getValue(DeclRef.getName())) {
            if (const auto AllocaInst = llvm::dyn_cast<llvm::AllocaInst>(Value))
            {
                return
                    Builder.CreateLoad(
                        llvm::Type::getDoubleTy(Handler.getContext()),
                        AllocaInst,
                        "loadedValue");
            }

            if (const auto GlobalVar =
                    llvm::dyn_cast<llvm::GlobalVariable>(Value))
            {
                return
                    Builder.CreateLoad(
                        llvm::Type::getDoubleTy(Handler.getContext()),
                        GlobalVar,
                        "loadedValue");
            }

            return Value;
        }

        Handler.getDiag().consume({
            .Level = DiagnosticLevel::Error,
            .Location = DeclRef.getNameLoc(),
            .Message =
                std::format("Variable \"{}\" is not defined", DeclRef.getName())
        });

        return std::nullopt;
    }

    auto
    Handler::codegen(AST::Stmt &Stmt,
                     llvm::IRBuilder<> &Builder,
                     LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        switch (Stmt.getKind()) {
            case AST::NodeKind::BinaryOperation:
                return
                    BinaryOperationCodegen(
                        llvm::cast<AST::BinaryOperation>(Stmt),
                        *this,
                        Builder,
                        ValueMap);
            case AST::NodeKind::UnaryOperation:
                return
                    UnaryOperationCodegen(llvm::cast<AST::UnaryOperation>(Stmt),
                                          *this,
                                          Builder,
                                          ValueMap);
            case AST::NodeKind::CharLiteral:
                return
                    CharLiteralCodegen(llvm::cast<AST::CharLiteral>(Stmt),
                                       *this,
                                       Builder,
                                       ValueMap);
            case AST::NodeKind::NumberLiteral:
                return
                    NumberLiteralCodegen(llvm::cast<AST::NumberLiteral>(Stmt),
                                         *this,
                                         Builder,
                                         ValueMap);
            case AST::NodeKind::FloatLiteral:
                __builtin_unreachable();
            case AST::NodeKind::StringLiteral:
                return
                    StringLiteralCodegen(llvm::cast<AST::StringLiteral>(Stmt),
                                         *this,
                                         Builder,
                                         ValueMap);
            case AST::NodeKind::DeclRefExpr:
                return
                    DeclRefExprCodegen(llvm::cast<AST::DeclRefExpr>(Stmt),
                                       *this,
                                       Builder,
                                       ValueMap);
            case AST::NodeKind::ParenExpr:
                return
                    ParenExprCodegen(llvm::cast<AST::ParenExpr>(Stmt),
                                     *this,
                                     Builder,
                                     ValueMap);
            case AST::NodeKind::VarDecl:
                return
                    VarDeclCodegen(llvm::cast<AST::VarDecl>(Stmt),
                                   *this,
                                   Builder,
                                   ValueMap);
            case AST::NodeKind::FunctionDecl:
                return
                    FunctionDeclCodegen(llvm::cast<AST::FunctionDecl>(Stmt),
                                        *this,
                                        Builder,
                                        ValueMap);
            case AST::NodeKind::CallExpr:
                return
                    CallExprCodegen(llvm::cast<AST::CallExpr>(Stmt),
                                    *this,
                                    Builder,
                                    ValueMap);
            case AST::NodeKind::IfStmt:
                return
                    IfStmtCodegen(llvm::cast<AST::IfExpr>(Stmt),
                                  *this,
                                  Builder,
                                  ValueMap);
            case AST::NodeKind::ReturnStmt:
                return
                    ReturnStmtCodegen(llvm::cast<AST::ReturnStmt>(Stmt),
                                      *this,
                                      Builder,
                                      ValueMap);
            case AST::NodeKind::CompoundStmt:
                return
                    CompoundStmtCodegen(llvm::cast<AST::CompoundStmt>(Stmt),
                                        *this,
                                        Builder,
                                        ValueMap);
            case AST::NodeKind::StructDecl:
            case AST::NodeKind::ArraySubscriptExpr:
            case AST::NodeKind::CastExpr:
            case AST::NodeKind::FieldDecl:
            case AST::NodeKind::ParamVarDecl:
            case AST::NodeKind::FieldExpr:
            case AST::NodeKind::EnumMemberDecl:
            case AST::NodeKind::LvalueNamedDecl:
            case AST::NodeKind::EnumDecl:
            case AST::NodeKind::ArrayDecl:
            case AST::NodeKind::ClosureDecl:
                __builtin_unreachable();
            case AST::NodeKind::ArrayDestructuredVarDecl:
            case AST::NodeKind::ObjectDestructuredVarDecl:
            case AST::NodeKind::ForStmt:
            case AST::NodeKind::CommaSepStmtList:
            case AST::NodeKind::ArrayType:
            case AST::NodeKind::ClosureType:
            case AST::NodeKind::EnumType:
            case AST::NodeKind::FunctionType:
            case AST::NodeKind::OptionalType:
            case AST::NodeKind::PointerType:
            case AST::NodeKind::StructType:
            case AST::NodeKind::ShapeType:
            case AST::NodeKind::UnionType:
                __builtin_unreachable();
        }

        __builtin_unreachable();
    }
}
