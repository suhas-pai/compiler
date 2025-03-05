/*
 * Parse/ParseExpr.h
 * © suhas pai
 */

#pragma once

#include "AST/Expr.h"
#include "AST/UnaryOperation.h"

#include "Parse/Context.h"

namespace Parse {
    [[nodiscard]] auto
    ParseUnaryOperation(ParseContext &Context,
                        const Lex::Token Token) noexcept
        -> AST::UnaryOperation *;

    [[nodiscard]] auto ParseLhs(ParseContext &Context) noexcept -> AST::Expr *;

    [[nodiscard]] auto
    ParseBinaryOperationWithRhsPrec(ParseContext &Context,
                                    AST::Expr *Lhs,
                                    uint32_t MinPrec) noexcept -> AST::Expr *;

    [[nodiscard]]
    auto ParseExpression(ParseContext &Context) noexcept -> AST::Expr *;
}
