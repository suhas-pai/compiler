/*
 * AST/Context.cpp
 */

#include "AST/Context.h"

namespace AST {
    auto Context::addDecl(AST::Decl *const Decl) noexcept -> bool {
        return DeclList.insert({ std::string(Decl->getName()), Decl }).second;
    }
}