/*
 * Parse/ParseExpr.h
 * © suhas pai
 */

#pragma once
#include "AST/Expr.h"

#include "Parse/Context.h"
#include "Parse/ParseError.h"

namespace Parse {
    [[nodiscard]]
    auto ParseLhs(ParseContext &Context, bool InPlaceOfStmt) noexcept
        -> std::expected<AST::Expr *, ParseError>;

    [[nodiscard]] auto
    ParseBinaryOperationWithRhsPrec(ParseContext &Context,
                                    AST::Expr *Lhs,
                                    uint32_t MinPrec,
                                    bool InPlaceOfStmt) noexcept
        -> std::expected<AST::Expr *, ParseError>;

    [[nodiscard]] auto ParseExpression(ParseContext &Context) noexcept
        -> std::expected<AST::Expr *, ParseError>;

    [[nodiscard]]
    auto ParseExpressionInPlaceOfStmt(ParseContext &Context) noexcept
        -> std::expected<AST::Expr *, ParseError>;
}
