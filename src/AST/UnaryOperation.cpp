/*
 * AST/UnaryOperation.cpp
 */

#include "AST/UnaryOperation.h"

namespace AST {
    std::optional<llvm::Value *>
    UnaryOperation::codegen(Backend::LLVM::Handler &Handler,
                            llvm::IRBuilder<> &Builder,
                            Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        const auto OperandOpt =
            this->Operand->codegen(Handler, Builder, ValueMap);

        if (!OperandOpt.has_value()) {
            return std::nullopt;
        }

        const auto Operand = OperandOpt.value();
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