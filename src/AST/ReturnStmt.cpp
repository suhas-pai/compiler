/*
 * AST/ReturnStmt.cpp
 */

#include "AST/ReturnStmt.h"
#include "Backend/LLVM/Handler.h"

namespace AST {
    llvm::Value *
    ReturnStmt::codegen(Backend::LLVM::Handler &Handler,
                        llvm::IRBuilder<> &Builder,
                        Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        return Builder.CreateRet(Value->codegen(Handler, Builder, ValueMap));
    }
}