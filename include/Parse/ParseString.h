/*
 * Parse/ParseString.h
 * © suhas pai
 */

#pragma once

#include "AST/CharLiteral.h"
#include "AST/StringLiteral.h"

#include "Parse/Context.h"

namespace Parse {
    auto ParseCharLiteral(ParseContext &Context, Lex::Token Token) noexcept
        -> AST::CharLiteral *;

    auto ParseStringLiteral(ParseContext &Context, Lex::Token Token) noexcept
        -> AST::StringLiteral *;
}