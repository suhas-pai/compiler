/*
 * AST/BinaryOperation.cpp
 */

#include "AST/BinaryOperation.h"
#include "Backend/LLVM/Vars.h"
#include "Parse/Operator.h"
#include "llvm/IR/Instructions.h"

namespace AST {
    llvm::Value *BinaryOperation::codegen() noexcept {
        const auto Left = getLhs()->codegen();
        const auto Right = getRhs()->codegen();

        if (Left == nullptr || Right == nullptr) {
            return nullptr;
        }

        switch (getOperator()) {
            case Parse::BinaryOperator::Add:
                return Builder.CreateAdd(Left, Right, "addtmp");
            case Parse::BinaryOperator::Subtract:
                return Builder.CreateSub(Left, Right, "subtmp");
            case Parse::BinaryOperator::Multiply:
                return Builder.CreateMul(Left, Right, "multmp");
            case Parse::BinaryOperator::Modulo:
                return Builder.CreateURem(Left, Right, "modtmp");
            case Parse::BinaryOperator::Divide:
                return Builder.CreateUDiv(Left, Right, "divtmp");
            case Parse::BinaryOperator::Equality:
                return Builder.CreateICmpEQ(Left, Right, "eqtmp");
            case Parse::BinaryOperator::Inequality:
                return Builder.CreateICmpNE(Left, Right, "nqtmp");
            case Parse::BinaryOperator::Power: {
                const auto LeftDouble =
                    Builder.CreateUIToFP(Left,
                                         llvm::Type::getDoubleTy(TheContext));
                const auto RightDouble =
                    Builder.CreateUIToFP(Right,
                                         llvm::Type::getDoubleTy(TheContext));

                return Builder.CreateCall(PowerFunc,
                                          {LeftDouble, RightDouble},
                                          "calltmp");
            }
        }

        assert(false && "Unknown binary operator");
    }
}