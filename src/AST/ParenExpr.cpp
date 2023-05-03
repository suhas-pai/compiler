/*
 * AST/ParenExpr.cpp
 */

#include "AST/ParenExpr.h"
#include "llvm/IR/Value.h"

namespace AST {
    llvm::Value *
    ParenExpr::codegen(Backend::LLVM::Handler &Handler,
                       llvm::IRBuilder<> &Builder,
                       Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        return ChildExpr->codegen(Handler, Builder, ValueMap);
    }
}