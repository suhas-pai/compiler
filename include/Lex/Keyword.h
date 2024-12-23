/*
 * Lex/Keyword.h
 */

#pragma once

#include <string_view>
#include <utility>

#include "ADT/SmallArrayMap.h"

namespace Lex {
    enum class Keyword {
        Let,
        Mut,
        Function,
        If,
        Else,
        Return,
        Volatile,
        Struct,
        Enum,
        And,
        Or,
        For,
        Inline,
        Comptime,
        NoInline,
    };

    const auto KeywordToLexemeMap =
        ADT::SmallArrayMap<Keyword, std::string_view, 15>({
            std::make_pair(Keyword::Let, "let"),
            std::make_pair(Keyword::Mut, "mut"),
            std::make_pair(Keyword::Function, "func"),
            std::make_pair(Keyword::If, "if"),
            std::make_pair(Keyword::Else, "else"),
            std::make_pair(Keyword::Return, "return"),
            std::make_pair(Keyword::Volatile, "volatile"),
            std::make_pair(Keyword::Struct, "struct"),
            std::make_pair(Keyword::Enum, "enum"),
            std::make_pair(Keyword::And, "and"),
            std::make_pair(Keyword::Or, "or"),
            std::make_pair(Keyword::For, "for"),
            std::make_pair(Keyword::Inline, "inline"),
            std::make_pair(Keyword::Comptime, "comptime"),
            std::make_pair(Keyword::NoInline, "noinline"),
        });
}