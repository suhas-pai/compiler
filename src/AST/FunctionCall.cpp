/*
 * AST/FunctionCall.cpp
 */

#include "AST/FunctionCall.h"
#include "AST/FunctionDecl.h"

#include "llvm/Support/Casting.h"

namespace AST {
    std::optional<llvm::Value *>
    FunctionCall::codegen(Backend::LLVM::Handler &Handler,
                          llvm::IRBuilder<> &Builder,
                          Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        const auto FuncValue = ValueMap.getValue(Name);
        if (FuncValue == nullptr) {
            if (const auto Diag = Handler.getDiag()) {
                Diag->emitError("Function \"" SV_FMT "\" is not defined",
                                SV_FMT_ARG(getName()));
            }

            return std::nullopt;
        }

        const auto FuncDecl =
            llvm::cast_if_present<AST::FunctionDecl>(Handler.getASTNode(Name));

        assert(FuncDecl != nullptr &&
               "Function not in Handler's NameToASTNodeMap");

        const auto FuncProto = FuncDecl->getPrototype();
        const auto FuncLLVM = llvm::cast<llvm::Function>(FuncValue);

        if (getArgs().size() != FuncProto->getParamList().size()) {
            if (const auto Diag = Handler.getDiag()) {
                Diag->emitError("%" PRIdPTR " arguments provided to function "
                                "\"" SV_FMT "\", expected %" PRIdPTR,
                                getArgs().size(),
                                SV_FMT_ARG(getName()),
                                FuncProto->getParamList().size());
            }

            return std::nullopt;
        }

        auto ArgsV = std::vector<llvm::Value *>();
        ArgsV.reserve(getArgs().size());

        for (auto I = unsigned(); I != getArgs().size(); ++I) {
            if (const auto ResultOpt =
                    Args[I]->codegen(Handler, Builder, ValueMap))
            {
                ArgsV.push_back(ResultOpt.value());
                continue;
            }

            return std::nullopt;
        }

        return Builder.CreateCall(FuncLLVM, ArgsV, "calltmp");
    }
}