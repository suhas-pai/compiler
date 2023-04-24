/*
 * AST/ParenExpr.cpp
 */

#include "AST/ParenExpr.h"
#include "llvm/IR/Value.h"

namespace AST {
    llvm::Value *ParenExpr::codegen() noexcept {
        return ChildExpr->codegen();
    }
}