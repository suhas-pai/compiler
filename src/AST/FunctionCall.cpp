/*
 * AST/FunctionCall.cpp
 */

#include "AST/FunctionCall.h"
#include "Backend/LLVM/Handler.h"

namespace AST {
    llvm::Value *
    FunctionCall::codegen(Backend::LLVM::Handler &Handler) noexcept {
        return nullptr;
    }
}