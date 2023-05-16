/*
 * AST/ParenExpr.cpp
 */

#include "AST/ParenExpr.h"
#include "llvm/IR/Value.h"

namespace AST {
    auto
    ParenExpr::codegen(Backend::LLVM::Handler &Handler,
                       llvm::IRBuilder<> &Builder,
                       Backend::LLVM::ValueMap &ValueMap) noexcept
        -> std::optional<llvm::Value *>
    {
        return ChildExpr->codegen(Handler, Builder, ValueMap);
    }
}