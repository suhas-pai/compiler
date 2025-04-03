/*
 * Parse/ParseExpr.h
 * © suhas pai
 */

#pragma once

#include "AST/Expr.h"
#include "Parse/Context.h"

namespace Parse {
    [[nodiscard]]
    auto ParseLhs(ParseContext &Context, bool InPlaceOfStmt) noexcept
        -> AST::Expr *;

    [[nodiscard]] auto
    ParseBinaryOperationWithRhsPrec(ParseContext &Context,
                                    AST::Expr *Lhs,
                                    uint32_t MinPrec,
                                    bool InPlaceOfStmt) noexcept -> AST::Expr *;

    [[nodiscard]]
    auto ParseExpression(ParseContext &Context) noexcept -> AST::Expr *;

    [[nodiscard]]
    auto ParseExpressionInPlaceOfStmt(ParseContext &Context) noexcept
        -> AST::Expr *;
}
