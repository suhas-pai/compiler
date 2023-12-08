/*
 * AST/BinaryOperation.cpp
 */

#include "AST/BinaryOperation.h"

namespace AST {
    std::optional<llvm::Value *>
    BinaryOperation::codegen(Backend::LLVM::Handler &Handler,
                             llvm::IRBuilder<> &Builder,
                             Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        const auto LeftOpt = getLhs()->codegen(Handler, Builder, ValueMap);
        if (!LeftOpt.has_value()) {
            return std::nullopt;
        }

        const auto RightOpt = getRhs()->codegen(Handler, Builder, ValueMap);
        if (!RightOpt.has_value()) {
            return std::nullopt;
        }

        auto &Context = Handler.getContext();

        const auto Left = LeftOpt.value();
        const auto Right = RightOpt.value();

        switch (getOperator()) {
            case Parse::BinaryOperator::Add:
                return Builder.CreateFAdd(Left, Right, /*Name=*/"addtmp");
            case Parse::BinaryOperator::Subtract:
                return Builder.CreateFSub(Left, Right, /*Name=*/"subtmp");
            case Parse::BinaryOperator::Multiply:
                return Builder.CreateFMul(Left, Right, /*Name=*/"multmp");
            case Parse::BinaryOperator::Modulo:
                return Builder.CreateFRem(Left, Right, /*Name=*/"modtmp");
            case Parse::BinaryOperator::Divide:
                return Builder.CreateFDiv(Left, Right, /*Name=*/"divtmp");
            case Parse::BinaryOperator::Equality:
                return Builder.CreateFCmpOEQ(Left, Right, /*Name=*/"eqtmp");
            case Parse::BinaryOperator::Inequality:
                return Builder.CreateFCmpONE(Left, Right, /*Name=*/"nqtmp");
            case Parse::BinaryOperator::Power: {
                const auto LeftDouble =
                    Builder.CreateUIToFP(Left,
                                         llvm::Type::getDoubleTy(Context));
                const auto RightDouble =
                    Builder.CreateUIToFP(Right,
                                         llvm::Type::getDoubleTy(Context));

                const auto PowFunc =
                    llvm::cast<llvm::Function>(ValueMap.getValue("pow"));

                if (PowFunc == nullptr) {
                    Handler.getDiag().emitError(
                        "** operator only supprted on JIT");
                    return std::nullopt;
                }

                return Builder.CreateCall(PowFunc,
                                          {LeftDouble, RightDouble},
                                          "powtmp");
            }
        }

        assert(false && "Unknown binary operator");
    }
}