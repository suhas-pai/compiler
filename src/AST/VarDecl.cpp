/*
 * AST/VarDecl.cpp
 */

#include "AST/VarDecl.h"

namespace AST {
    llvm::Value *VarDecl::codegen(Backend::LLVM::Handler &Handler) noexcept {
        return InitExpr->codegen(Handler);
    }
}