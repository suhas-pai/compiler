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

        if (const auto Var = Handler.getNameToASTNodeMap().find(Name);
            Var != Handler.getNameToASTNodeMap().end())
        {
            return Var->second->codegen(Handler, ValueMap);
        }

        if (const auto Diag = Handler.getDiag()) {
            Diag->emitError("Variable \"" SV_FMT "\" not defined",
                            SV_FMT_ARG(Name));
        }

        return nullptr;
    }
}