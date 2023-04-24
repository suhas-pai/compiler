/*
 * AST/UnaryOperation.cpp
 */

#include "AST/UnaryOperation.h"
#include "Backend/LLVM/Vars.h"

namespace AST {
    llvm::Value *UnaryOperation::codegen() noexcept {
        const auto Operand = this->Operand->codegen();
        if (Operand == nullptr) {
            return nullptr;
        }

        switch (this->Operator) {
            case Parse::UnaryOperator::Negate:
                return Builder.CreateNeg(Operand);
            case Parse::UnaryOperator::LogicalNot:
                assert(false && "Logical-Not not yet supported");
            case Parse::UnaryOperator::BitwiseNot:
                return Builder.CreateNot(Operand);
            case Parse::UnaryOperator::Increment: {
                const auto Value =
                    llvm::ConstantInt::get(Operand->getType(), 1);
                return Builder.CreateAdd(Operand, Value);
            }
            case Parse::UnaryOperator::Decrement: {
                const auto Value =
                    llvm::ConstantInt::get(Operand->getType(), 1);

                return Builder.CreateSub(Operand, Value);
            }
            case Parse::UnaryOperator::AddressOf:
                assert(false && "AddressOf not yet supported");
            case Parse::UnaryOperator::Dereference:
                assert(false && "Dereference not yet supported");
        }

        assert(false && "Unknown UnaryOperator");
    }
}