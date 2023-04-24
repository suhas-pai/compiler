/*
 * AST/CharacterLiteral.cpp
 */

#include "AST/CharLiteral.h"
#include "Backend/LLVM/Vars.h"

namespace AST {
    llvm::Value *CharLiteral::codegen() noexcept {
        return llvm::ConstantInt::get(llvm::Type::getInt8Ty(TheContext),
                                      Value,
                                      /*IsSigned=*/true);
    }
}