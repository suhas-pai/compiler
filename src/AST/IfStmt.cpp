/*
 * AST/IfStmt.cpp
 */

#include "AST/IfStmt.h"

namespace AST {
    std::optional<llvm::Value *>
    IfStmt::codegen(Backend::LLVM::Handler &Handler,
                    llvm::IRBuilder<> &Builder,
                    Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        auto CondValueOpt = Cond->codegen(Handler, Builder, ValueMap);
        if (!CondValueOpt.has_value()) {
            return std::nullopt;
        }

        auto CondValue = CondValueOpt.value();
        auto &Context = Handler.getContext();

        // Convert condition to a bool by comparing non-equal to 0.0.
        const auto ZeroFP =  llvm::ConstantFP::get(Context, llvm::APFloat(0.0));
        CondValue = Builder.CreateFCmpONE(CondValue, ZeroFP, "ifcond");

        const auto TheFunction = Builder.GetInsertBlock()->getParent();

        // Create blocks for the then and else cases.  Insert the 'then' block
        // at the end of the function.

        auto ThenBB = llvm::BasicBlock::Create(Context, "then", TheFunction);
        auto ElseBB = llvm::BasicBlock::Create(Context, "else");

        // Emit then value.
        const auto CondBrValue =
            Builder.CreateCondBr(CondValue, ThenBB, ElseBB);

        auto ThenIRBuilder = llvm::IRBuilder<>(ThenBB);
        if (!Then->codegen(Handler, ThenIRBuilder, ValueMap).has_value()) {
            ThenBB->eraseFromParent();
            ElseBB->eraseFromParent();

            CondBrValue->eraseFromParent();
            CondValue->deleteValue();

            return std::nullopt;
        }

        // Emit else block.
        TheFunction->insert(TheFunction->end(), ElseBB);
        if (Else != nullptr) {
            auto ElseIRBuilder = llvm::IRBuilder<>(ElseBB);
            const auto ElseValueOpt =
                Else->codegen(Handler, ElseIRBuilder, ValueMap);

            if (!ElseValueOpt.has_value()) {
                ThenBB->eraseFromParent();
                ElseBB->eraseFromParent();

                CondBrValue->eraseFromParent();
                CondValue->deleteValue();

                return std::nullopt;
            }
        }

        return TheFunction;
    }
}