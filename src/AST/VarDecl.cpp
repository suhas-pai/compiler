/*
 * AST/VarDecl.cpp
 */

#include "AST/VarDecl.h"
#include "Backend/LLVM/Vars.h"

namespace AST {
    llvm::Value *VarDecl::codegen() noexcept {
        return InitExpr->codegen();
    }
}