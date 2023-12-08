/*
 * AST/ReturnStmt.cpp
 */

#include "AST/ReturnStmt.h"
#include "Backend/LLVM/Handler.h"

namespace AST {
    std::optional<llvm::Value *>
    ReturnStmt::codegen(Backend::LLVM::Handler &Handler,
                        llvm::IRBuilder<> &Builder,
                        Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        if (Value == nullptr) {
            return Builder.CreateRetVoid();
        }

        if (const auto ResultOpt = Value->codegen(Handler, Builder, ValueMap)) {
            const auto Result = ResultOpt.value();

            // For stack variables, create a load instruction and return its
            // result

            if (const auto AllocaInst =
                    llvm::dyn_cast<llvm::AllocaInst>(Result))
            {
                const auto LoadedValue =
                    Builder.CreateLoad(
                        llvm::Type::getDoubleTy(Handler.getContext()),
                        AllocaInst,
                        "loadedValue");

                return Builder.CreateRet(LoadedValue);
            }

            // Otherwise, directly return the value
            return Builder.CreateRet(Result);
        }

        return std::nullopt;
    }
}