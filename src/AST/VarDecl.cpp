/*
 * AST/VarDecl.cpp
 */

#include "AST/VarDecl.h"

namespace AST {
    llvm::Value *
    VarDecl::codegen(Backend::LLVM::Handler &Handler,
                     Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        if (ValueMap.getValue(getName()) != nullptr) {
            if (const auto Diag = Handler.getDiag()) {
                Diag->emitError("Variable \'" SV_FMT "\' is already defined",
                                SV_FMT_ARG(getName()));
            }

            return nullptr;
        }

        if (const auto Result = InitExpr->codegen(Handler, ValueMap)) {
            Result->setName(Name);
            return Result;
        }

        return nullptr;
    }
}