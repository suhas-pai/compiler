/*
 * AST/VarDecl.cpp
 */

#include "AST/VarDecl.h"

#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Type.h"

namespace AST {
    std::optional<llvm::Value *>
    VarDecl::codegen(Backend::LLVM::Handler &Handler,
                     llvm::IRBuilder<> &Builder,
                     Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        if (ValueMap.getValue(getName()) != nullptr) {
            Handler.getDiag().emitError(
                "Variable \'" SV_FMT "\' is already defined",
                SV_FMT_ARG(getName()));

            return std::nullopt;
        }

        const auto ResultOpt = InitExpr->codegen(Handler, Builder, ValueMap);
        if (!ResultOpt.has_value()) {
            return std::nullopt;
        }

        if (getLinkage() == Linkage::External) {
            const auto Type = llvm::Type::getDoubleTy(Handler.getContext());
            const auto gVar =
                new llvm::GlobalVariable(
                    Type,
                    /*isConstant=*/false,
                    llvm::GlobalVariable::LinkageTypes::ExternalLinkage,
                    /*Initializer=*/nullptr,
                    getName());

            return gVar;
        }

        const auto FuncParent = Builder.GetInsertBlock()->getParent();
        auto TmpB =
            llvm::IRBuilder<>(&FuncParent->getEntryBlock(),
                              FuncParent->getEntryBlock().begin());

        const auto DoubleTy = llvm::Type::getDoubleTy(Handler.getContext());
        const auto AllocaBlock =
            TmpB.CreateAlloca(DoubleTy, nullptr, getName());

        Builder.CreateStore(ResultOpt.value(), AllocaBlock);
        return AllocaBlock;
    }
}