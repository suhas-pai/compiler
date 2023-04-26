/*
 * AST/ParenExpr.cpp
 */

#include "AST/ParenExpr.h"
#include "llvm/IR/Value.h"

namespace AST {
    llvm::Value *ParenExpr::codegen(Backend::LLVM::Handler &Handler) noexcept {
        return ChildExpr->codegen(Handler);
    }
}