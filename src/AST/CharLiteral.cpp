/*
 * AST/CharacterLiteral.cpp
 */

#include "AST/CharLiteral.h"
#include "Backend/LLVM/Handler.h"

namespace AST {
    llvm::Value *
    CharLiteral::codegen(Backend::LLVM::Handler &Handler,
                         Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        auto &Context = Handler.getContext();
        return llvm::ConstantInt::get(llvm::Type::getInt8Ty(Context),
                                      Value,
                                      /*IsSigned=*/false);
    }
}