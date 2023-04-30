/*
 * AST/VarDecl.cpp
 */

#include "AST/VarDecl.h"

namespace AST {
    llvm::Value *
    VarDecl::codegen(Backend::LLVM::Handler &Handler,
                     Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        if (const auto Result = InitExpr->codegen(Handler, ValueMap)) {
            Result->setName(Name);
            return Result;
        }

        return nullptr;
    }
}