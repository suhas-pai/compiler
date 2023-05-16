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
            return Builder.CreateRet(ResultOpt.value());
        }

        return std::nullopt;
    }
}