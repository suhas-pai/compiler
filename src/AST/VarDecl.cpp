/*
 * AST/VarDecl.cpp
 */

#include "AST/VarDecl.h"

namespace AST {
    llvm::Value *
    VarDecl::codegen(Backend::LLVM::Handler &Handler,
                     Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        if (Handler.getNameToASTNodeMap().contains(Name)) {
            if (const auto Diag = Handler.getDiag()) {
                Diag->emitError("Variable \"" SV_FMT "\" already defined",
                                SV_FMT_ARG(Name));
            }

            return nullptr;
        }

        const auto Result = InitExpr->codegen(Handler, ValueMap);
        if (Result == nullptr) {
            return nullptr;
        }

        Result->setName(Name);
        Handler.addASTNode(Name, *this);

        return Result;
    }
}