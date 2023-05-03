/*
 * AST/FunctionDecl.cp
 */

#include "AST/FunctionDecl.h"
#include "AST/NodeKind.h"
#include "llvm/IR/Verifier.h"

namespace AST {
    llvm::Value *
    FunctionDecl::finishPrototypeCodegen(
        Backend::LLVM::Handler &Handler,
        llvm::IRBuilder<> &Builder,
        Backend::LLVM::ValueMap &ValueMap,
        llvm::Value *const ProtoCodegen) noexcept
    {
        if (IsExternal) {
            return ProtoCodegen;
        }

        assert(Body != nullptr && "FunctionDecl's body is null");

        const auto Function = llvm::cast<llvm::Function>(ProtoCodegen);
        const auto BB =
            llvm::BasicBlock::Create(Handler.getContext(),
                                     "entry",
                                     Function);

        for (auto &Arg : Function->args()) {
            ValueMap.addValue(Arg.getName(), &Arg);
        }

        auto BodyIRBuilder = llvm::IRBuilder(BB);
        const auto RetVal = Body->codegen(Handler, BodyIRBuilder, ValueMap);

        if (RetVal == nullptr) {
            Function->removeFromParent();
            return nullptr;
        }

        for (auto &Arg : Function->args()) {
            ValueMap.removeValue(Arg.getName());
        }

        // Finish off the function.
        if (Body->getKind() != AST::NodeKind::IfStmt &&
            Body->getKind() != AST::NodeKind::ReturnStmt)
        {
            BodyIRBuilder.CreateRet(RetVal);
        }

        // Validate the generated code, checking for consistency.
        llvm::verifyFunction(*Function);
        Handler.getModule().print(llvm::outs(), nullptr);

        // Run the optimizer on the function.
        Handler.getFPM().run(*Function);
        return Function;
    }

    llvm::Value *
    FunctionDecl::codegen(Backend::LLVM::Handler &Handler,
                          llvm::IRBuilder<> &Builder,
                          Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        const auto ProtoCodegen =
            getPrototype()->codegen(Handler, Builder, ValueMap);

        return finishPrototypeCodegen(Handler, Builder, ValueMap, ProtoCodegen);
    }
}