/*
 * AST/VarDecl.cpp
 */

#include "AST/VarDecl.h"

namespace AST {
    llvm::Value *
    VarDecl::codegen(Backend::LLVM::Handler &Handler,
                     Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        const auto Result = InitExpr->codegen(Handler, ValueMap);
        if (Result == nullptr) {
            return nullptr;
        }

        Result->setName(Name);
        return Result;
    }
}