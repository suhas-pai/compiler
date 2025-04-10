/*
 * Parse/ParseStmt.h
 * © suhas pai
 */

#pragma once

#include "AST/CompoundStmt.h"
#include "AST/IfExpr.h"
#include "AST/ReturnStmt.h"

#include "Parse/Context.h"
#include "Parse/ParseError.h"

namespace Parse {
    [[nodiscard]] auto
    ParseCompoundStmt(ParseContext &Context,
                      Lex::Token CurlyToken) noexcept
        -> std::expected<AST::CompoundStmt *, ParseError>;

    [[nodiscard]]
    auto ParseIfStmt(ParseContext &Context, Lex::Token IfToken) noexcept
        -> std::expected<AST::IfExpr *, ParseError>;

    [[nodiscard]] auto
    ParseReturnStmt(ParseContext &Context, Lex::Token ReturnToken) noexcept
        -> std::expected<AST::ReturnStmt *, ParseError>;

    [[nodiscard]] auto ParseStmt(ParseContext &Context) noexcept
        -> std::expected<AST::Stmt *, ParseError>;
}
