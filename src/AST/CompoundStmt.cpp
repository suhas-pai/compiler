/*
 * AST/CompoundStmt.cpp
 */

#include "AST/CompoundStmt.h"
#include "AST/Decl.h"

namespace AST {
    std::optional<llvm::Value *>
    CompoundStmt::codegen(Backend::LLVM::Handler &Handler,
                          llvm::IRBuilder<> &Builder,
                          Backend::LLVM::ValueMap &ValueMap) noexcept
    {
        // Create blocks for the then and else cases. Insert the 'then' block
        // at the end of the function.

        auto AddedDecls = std::vector<std::string_view>();
        for (const auto &Stmt : getStmtList()) {
            if (const auto ResultOpt =
                    Stmt->codegen(Handler, Builder, ValueMap))
            {
                if (const auto Decl = llvm::dyn_cast<AST::Decl>(Stmt)) {
                    ValueMap.addValue(Decl->getName(), ResultOpt.value());
                    AddedDecls.push_back(Decl->getName());
                }

                continue;
            }

            for (const auto Name : AddedDecls) {
                ValueMap.removeValue(Name);
            }

            return std::nullopt;
        }

        for (const auto Name : AddedDecls) {
            ValueMap.removeValue(Name);
        }

        return nullptr;
    }
}