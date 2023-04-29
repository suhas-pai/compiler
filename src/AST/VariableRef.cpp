/*
 * AST/VariableRef.cpp
 */

#include "AST/VariableRef.h"
#include "Backend/LLVM/Handler.h"

namespace AST {
    llvm::Value *
    VariableRef::codegen(Backend::LLVM::Handler &Handler) noexcept {
        if (const auto Var = Handler.getNamedValues().find(Name);
            Var != Handler.getNamedValues().end())
        {
            return Var->second->codegen(Handler);
        }

        Handler.getDiag().emitError("Variable %s not defined", Name.data());
        return nullptr;
    }
}