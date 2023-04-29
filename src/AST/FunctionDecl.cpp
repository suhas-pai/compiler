/*
 * AST/FunctionDecl.cp
 */

#include "AST/FunctionDecl.h"
#include "AST/ExprKind.h"
#include "AST/VarDecl.h"
#include "Backend/LLVM/Handler.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Casting.h"

namespace AST {
    llvm::Value *
    FunctionDecl::codegen(Backend::LLVM::Handler &Handler) noexcept {
        const auto Function =
            static_cast<llvm::Function *>(getPrototype()->codegen(Handler));

        const auto BB =
            llvm::BasicBlock::Create(Handler.getContext(), "entry", Function);

        Handler.getBuilder().SetInsertPoint(BB);
        for (auto &Arg : Function->args()) {
            Handler.getNamedValuesRef().insert({
                std::string(Arg.getName()), nullptr });
        }

        const auto RetVal = Body->codegen(Handler);
        for (auto &Arg : Function->args()) {
            Handler.getNamedValuesRef().erase(std::string(Arg.getName()));
        }

        if (RetVal == nullptr) {
            Function->removeFromParent();
            return nullptr;
        }

        // Finish off the function.
        Handler.getBuilder().CreateRet(RetVal);

        // Validate the generated code, checking for consistency.
        llvm::verifyFunction(*Function);

        // Run the optimizer on the function.
        Handler.getFPM().run(*Function);

        return Function;
    }
}