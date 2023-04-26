/*
 * AST/FunctionDecl.cp
 */

#include "AST/FunctionDecl.h"
#include "Backend/LLVM/Handler.h"
#include "llvm/IR/Verifier.h"

namespace AST {
    llvm::Value *
    FunctionDecl::codegen(Backend::LLVM::Handler &Handler) noexcept {
        const auto Function =
            static_cast<llvm::Function *>(getPrototype()->codegen(Handler));

        const auto BB =
            llvm::BasicBlock::Create(Handler.getContext(), "entry", Function);

        Handler.getBuilder().SetInsertPoint(BB);
        Handler.getNamedValuesRef().clear();

        for (auto &Arg : Function->args()) {
            Handler.getNamedValuesRef().insert(
                std::make_pair(Arg.getName(), &Arg));
        }

        if (const auto RetVal = Body->codegen(Handler)) {
            // Finish off the function.
            Handler.getBuilder().CreateRet(RetVal);

            // Validate the generated code, checking for consistency.
            llvm::verifyFunction(*Function);

            // Run the optimizer on the function.
            Handler.getFPM().run(*Function);

            return Function;
        }

        return nullptr;
    }
}