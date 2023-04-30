/*
 * AST/FunctionDecl.cp
 */

#include "AST/FunctionDecl.h"
#include "llvm/IR/Verifier.h"

namespace AST {
    llvm::Value *
    FunctionDecl::codegen(Backend::LLVM::Handler &Handler,
                          Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        const auto Proto = getPrototype();
        const auto Function =
            llvm::cast<llvm::Function>(Proto->codegen(Handler, ValueMap));
        const auto BB =
            llvm::BasicBlock::Create(Handler.getContext(),
                                     "entry",
                                     Function);

        const auto InsertPoint = Handler.getBuilder().GetInsertBlock();
        Handler.getBuilder().SetInsertPoint(BB);

        for (auto &Arg : Function->args()) {
            ValueMap.addValue(Arg.getName(), &Arg);
        }

        const auto RetVal = Body->codegen(Handler, ValueMap);
        if (RetVal == nullptr) {
            Function->removeFromParent();
            return nullptr;
        }

        for (auto &Arg : Function->args()) {
            ValueMap.removeValue(Arg.getName());
        }

        // Finish off the function.
        Handler.getBuilder().CreateRet(RetVal);
        Handler.getBuilder().SetInsertPoint(InsertPoint);

        // Validate the generated code, checking for consistency.
        llvm::verifyFunction(*Function);

        // Run the optimizer on the function.
        Handler.getFPM().run(*Function);
        return Function;
    }
}