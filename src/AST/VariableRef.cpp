/*
 * AST/VariableRef.cpp
 */

#include "AST/VariableRef.h"
#include "Backend/LLVM/Handler.h"

namespace AST {
    llvm::Value *
    VariableRef::codegen(Backend::LLVM::Handler &Handler,
                         Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        if (const auto Value = ValueMap.getValue(Name)) {
            return Value;
        }

        if (const auto Diag = Handler.getDiag()) {
            Diag->emitError("Variable \"" SV_FMT "\" is not defined",
                            SV_FMT_ARG(Name));
        }

        return nullptr;
    }
}