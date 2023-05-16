/*
 * AST/FunctionPrototype.cpp
 */

#include "AST/FunctionProtoype.h"
#include "Backend/LLVM/Handler.h"

namespace AST {
    std::optional<llvm::Value *>
    FunctionPrototype::codegen(Backend::LLVM::Handler &Handler,
                               llvm::IRBuilder<> &Builder,
                               Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        auto &Context = Handler.getContext();
        auto &Module = Handler.getModule();

        const auto DoubleTy = llvm::Type::getDoubleTy(Context);
        const auto ParamList = std::vector(this->ParamList.size(), DoubleTy);
        const auto FT =
            llvm::FunctionType::get(DoubleTy, ParamList,/*isVarArg=*/false);
        const auto F =
            llvm::Function::Create(FT,
                                   llvm::Function::ExternalLinkage,
                                   Name,
                                   Module);

        // Set names for all arguments.
        auto Idx = unsigned();
        for (auto &Arg : F->args()) {
            Arg.setName(this->ParamList[Idx++].getName());
        }

        return F;
    }
}