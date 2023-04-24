/*
 * AST/IntegerLiteral.cpp
 */

#include "AST/IntegerLiteral.h"
#include "Backend/LLVM/Vars.h"

namespace AST {
    llvm::Value *IntegerLiteral::codegen() noexcept {
        return llvm::ConstantInt::get(llvm::Type::getInt64Ty(TheContext),
                                      Number.UInt,
                                      /*IsSigned=*/true);
    }
}