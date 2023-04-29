/*
 * AST/VarDecl.cpp
 */

#include "AST/VarDecl.h"

namespace AST {
    llvm::Value *VarDecl::codegen(Backend::LLVM::Handler &Handler) noexcept {
        if (Handler.getNamedValues().contains(Name)) {
            Handler.getDiag().emitError("Variable \"" SV_FMT "\" already "
                                        "defined",
                                        SV_FMT_ARG(Name));
            return nullptr;
        }

        const auto Result = InitExpr->codegen(Handler);
        if (Result == nullptr) {
            return nullptr;
        }

        Result->setName(this->getName());
        Handler.getNamedValuesRef().insert({Name, InitExpr});

        return Result;
    }
}