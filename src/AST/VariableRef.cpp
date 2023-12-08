/*
 * AST/VariableRef.cpp
 */

#include "AST/VariableRef.h"

namespace AST {
    std::optional<llvm::Value *>
    VariableRef::codegen(Backend::LLVM::Handler &Handler,
                         llvm::IRBuilder<> &Builder,
                         Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        if (const auto Value = ValueMap.getValue(Name)) {
            if (const auto AllocaInst =
                    llvm::dyn_cast<llvm::AllocaInst>(Value))
            {
                return
                    Builder.CreateLoad(
                        llvm::Type::getDoubleTy(Handler.getContext()),
                        AllocaInst,
                        "loadedValue");
            }

            return Value;
        }

        Handler.getDiag().emitError("Variable \"" SV_FMT "\" is not defined",
                                    SV_FMT_ARG(Name));

        return std::nullopt;
    }
}