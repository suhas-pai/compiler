/*
 * AST/StringLiteral.cpp
 */

#include "AST/StringLiteral.h"
#include "Backend/LLVM/Handler.h"

namespace AST {
    llvm::Value *
    StringLiteral::codegen(Backend::LLVM::Handler &Handler,
                           llvm::IRBuilder<> &Builder,
                           Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        return llvm::ConstantDataArray::getString(Handler.getContext(), Value);
    }
}