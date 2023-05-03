/*
 * AST/IfStmt.cpp
 */

#include "AST/IfStmt.h"

namespace AST {
    llvm::Value *
    IfStmt::codegen(Backend::LLVM::Handler &Handler,
                    llvm::IRBuilder<> &Builder,
                    Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        auto CondValue = Cond->codegen(Handler, Builder, ValueMap);
        if (CondValue == nullptr) {
            return nullptr;
        }

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
        if (Then->codegen(Handler, ThenIRBuilder, ValueMap) == nullptr) {
            ThenBB->eraseFromParent();
            ElseBB->eraseFromParent();

            CondBrValue->eraseFromParent();
            CondValue->deleteValue();

            return nullptr;
        }

        // Emit else block.
        TheFunction->insert(TheFunction->end(), ElseBB);

        auto ElseValue = static_cast<llvm::Value *>(nullptr);
        if (Else != nullptr) {
            auto ElseIRBuilder = llvm::IRBuilder<>(ElseBB);
            ElseValue = Else->codegen(Handler, ElseIRBuilder, ValueMap);

            if (ElseValue == nullptr) {
                ThenBB->eraseFromParent();
                ElseBB->eraseFromParent();

                CondBrValue->eraseFromParent();
                CondValue->deleteValue();

                return nullptr;
            }
        }

        return TheFunction;
    }
}