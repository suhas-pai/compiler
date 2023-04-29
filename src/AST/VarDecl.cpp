/*
 * AST/VarDecl.cpp
 */

#include "AST/VarDecl.h"
#include "Basic/Macros.h"

namespace AST {
    llvm::Value *VarDecl::codegen(Backend::LLVM::Handler &Handler) noexcept {
        if (Handler.getNamedValues().contains(Name)) {
            Handler.getDiag().emitError("Variable \"" SV_FMT "\" already "
                                        "exists",
                                        SV_FMT_ARG(Name));
            return nullptr;
        }

        const auto Result = InitExpr->codegen(Handler);

        Result->setName(this->getName());
        Handler.getNamedValuesRef().insert({Name, InitExpr});

        return Result;
    }
}