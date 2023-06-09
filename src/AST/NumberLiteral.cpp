/*
 * AST/NumberLiteral.cpp
 */

#include "AST/NumberLiteral.h"

namespace AST {
    std::optional<llvm::Value *>
    NumberLiteral::codegen(Backend::LLVM::Handler &Handler,
                           llvm::IRBuilder<> &Builder,
                           Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        auto &Context = Handler.getContext();
        #if 0
        return llvm::ConstantInt::get(llvm::Type::getInt64Ty(Context),
                                      Number.UInt,
                                      /*IsSigned=*/true);
        #endif

        const auto FloatVal = (double)Number.UInt;
        return llvm::ConstantFP::get(Context, llvm::APFloat(FloatVal));
    }
}