/*
 * AST/StringLiteral.cpp
 */

#include "AST/StringLiteral.h"
#include "Backend/LLVM/Vars.h"

namespace AST {
    llvm::Value *StringLiteral::codegen() noexcept {
        return llvm::ConstantDataArray::getString(TheContext, Value);
    }
}