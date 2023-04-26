/*
 * AST/FunctionPrototype.cpp
 */

#include "AST/FunctionProtoype.h"
#include "Backend/LLVM/Handler.h"

namespace AST {
    llvm::Value *
    FunctionPrototype::codegen(Backend::LLVM::Handler &Handler) noexcept {
        if (const auto Value = Handler.getValueForName(Name)) {
            return Value;
        }

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
        unsigned Idx = 0;
        for (auto &Arg : F->args())
            Arg.setName(this->ParamList[Idx++].getName());

        return F;
    }
}