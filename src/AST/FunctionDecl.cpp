/*
 * AST/FunctionDecl.cp
 */

#include "AST/FunctionDecl.h"
#include "AST/NodeKind.h"
#include "AST/ReturnStmt.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Verifier.h"

namespace AST {
    std::optional<llvm::Value *>
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
        const auto BodyValOpt = Body->codegen(Handler, BodyIRBuilder, ValueMap);

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
        Handler.getModule().print(llvm::outs(), nullptr);

        // Run the optimizer on the function.
        Handler.getFPM().run(*Function);
        return Function;
    }

    std::optional<llvm::Value *>
    FunctionDecl::codegen(Backend::LLVM::Handler &Handler,
                          llvm::IRBuilder<> &Builder,
                          Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        const auto ProtoCodegenOpt =
            getPrototype()->codegen(Handler, Builder, ValueMap);

        if (!ProtoCodegenOpt.has_value()) {
            return std::nullopt;
        }

        const auto ProtoCodegen = ProtoCodegenOpt.value();
        return finishPrototypeCodegen(Handler, Builder, ValueMap, ProtoCodegen);
    }
}