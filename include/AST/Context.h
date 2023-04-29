/*
 * AST/Context.h
 */

#pragma once

#include <unordered_map>
#include "Expr.h"

namespace AST {
    struct Context {
    protected:
        std::unordered_map<std::string, AST::Expr *> Symbols;
    public:
        explicit Context() noexcept = default;
    };
}