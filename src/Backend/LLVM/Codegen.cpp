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
        if (BinOp.getOperator() == Parse::BinaryOperator::Assignment) {
            const auto VarRef =
                llvm::dyn_cast<AST::DeclRefExpr>(&BinOp.getLhs());

            if (VarRef == nullptr) {
                Handler.getDiag().emitError(
                    BinOp.getLoc(),
                    "Left-side of assignment is not a variable");

                return std::nullopt;
            }

            const auto VarName = VarRef->getName();
            const auto VarRefValue = ValueMap.getValue(VarName);

            if (VarRefValue == nullptr) {
                Handler.getDiag().emitError(
                    VarRef->getNameLoc(),
                    "Variable \"" SV_FMT "\" is not defined",
                    SV_FMT_ARG(VarName));

                return std::nullopt;
            }

            if (const auto VarDecl =
                    llvm::dyn_cast<AST::VarDecl>(Handler.getASTNode(VarName)))
            {
                if (!VarDecl->getQualifiers().isMutable()) {
                    Handler.getDiag().emitError(
                        VarRef->getNameLoc(),
                        "Variable \"" SV_FMT "\" is constant",
                        SV_FMT_ARG(VarName));

                    return std::nullopt;
                }
            }

            const auto RhsValueOpt =
                Handler.codegen(BinOp.getRhs(), Builder, ValueMap);

            if (!RhsValueOpt.has_value()) {
                return std::nullopt;
            }

            const auto RhsValue = RhsValueOpt.value();
            Builder.CreateStore(RhsValue, VarRefValue);

            return RhsValue;
        }

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
                    Handler.getDiag().emitError(
                        BinOp.getLoc(),
                        "** operator only supprted on JIT");

                    return std::nullopt;
                }

                return Builder.CreateCall(PowFunc,
                                          {LeftDouble, RightDouble},
                                          "powtmp");
            }
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
    CompoundStmtCodegen(AST::CompoundStmt &CompountStmt,
                        Handler &Handler,
                        llvm::IRBuilder<> &Builder,
                        ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        // Create blocks for the then and else cases. Insert the 'then' block at
        // the end of the function.

        auto AddedDecls = std::vector<std::string_view>();
        for (const auto &Stmt : CompountStmt.getStmtList()) {
            if (const auto ResultOpt =
                    Handler.codegen(*Stmt, Builder, ValueMap))
            {
                if (const auto Decl = llvm::dyn_cast<AST::NamedDecl>(Stmt)) {
                    ValueMap.addValue(Decl->getName(), ResultOpt.value());
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
        const auto FuncValue = ValueMap.getValue(FuncCall.getName());
        if (FuncValue == nullptr) {
            Handler.getDiag().emitError(
                FuncCall.getNameLoc(),
                "Function \"" SV_FMT "\" is not defined",
                SV_FMT_ARG(FuncCall.getName()));

            return std::nullopt;
        }

        const auto Node = Handler.getASTNode(FuncCall.getName());
        DIAG_ASSERT(Handler.getDiag(),
                    Node != nullptr,
                    "Function \"" SV_FMT "\" is not in AST, but is in "
                    "ValueMap",
                    SV_FMT_ARG(FuncCall.getName()));

        const auto FuncDecl = llvm::dyn_cast<AST::FunctionDecl>(Node);
        if (FuncDecl == nullptr) {
            Handler.getDiag().emitError(
                FuncCall.getNameLoc(),
                "\"" SV_FMT "\" is not a function",
                SV_FMT_ARG(FuncCall.getName()));

            return std::nullopt;
        }

        const auto FuncLLVM = llvm::cast<llvm::Function>(FuncValue);
        if (FuncCall.getArgs().size() != FuncDecl->getParamList().size()) {
            Handler.getDiag().emitError(
                FuncCall.getNameLoc(),
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
            ValueMap.addValue(Arg.getName(), &Arg);
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
    IfStmtCodegen(AST::IfStmt &IfStmt,
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

        const auto FloatVal = (double)NumLit.getNumber().UInt;
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
                                                  StrLit.getValue());
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
                Handler.getDiag().emitError(UnaryOp.getLoc(),
                                            "AddressOf not yet supported");
                return std::nullopt;
            case Parse::UnaryOperator::Dereference:
                Handler.getDiag().emitError(UnaryOp.getLoc(),
                                            "Dereference not yet supported");
                return std::nullopt;
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
            Handler.getDiag().emitError(
                VarDecl.getNameLoc(),
                "Variable \'" SV_FMT "\' is already defined",
                SV_FMT_ARG(VarDecl.getName()));

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

        Handler.getDiag().emitError(DeclRef.getNameLoc(),
                                    "Variable \"" SV_FMT "\" is not defined",
                                    SV_FMT_ARG(DeclRef.getName()));

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
            case AST::NodeKind::Paren:
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
                    IfStmtCodegen(llvm::cast<AST::IfStmt>(Stmt),
                                  *this,
                                  Builder,
                                  ValueMap);
            case AST::NodeKind::ReturnStmt:
                return
                    ReturnStmtCodegen(llvm::cast<AST::ReturnStmt>(Stmt),
                                      *this,
                                      Builder,
                                      ValueMap);
            case AST::NodeKind::CompountStmt:
                return
                    CompoundStmtCodegen(llvm::cast<AST::CompoundStmt>(Stmt),
                                        *this,
                                        Builder,
                                        ValueMap);
            case AST::NodeKind::StructDecl:
            case AST::NodeKind::ArraySubscriptExpr:
            case AST::NodeKind::FieldDecl:
            case AST::NodeKind::ParamVarDecl:
            case AST::NodeKind::TypeRef:
            case AST::NodeKind::FieldExpr:
            case AST::NodeKind::EnumMemberDecl:
            case AST::NodeKind::EnumDecl:
            case AST::NodeKind::ArrayDecl:
            case AST::NodeKind::LvalueNamedDecl:
            case AST::NodeKind::LambdaDecl:
                __builtin_unreachable();
        }

        __builtin_unreachable();
    }
}
