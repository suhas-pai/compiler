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
    };

    const auto KeywordToLexemeMap =
        ADT::SmallArrayMap<Keyword, std::string_view, 7>({
            std::make_pair(Keyword::Let, "let"),
            std::make_pair(Keyword::Mut, "mut"),
            std::make_pair(Keyword::Function, "func"),
            std::make_pair(Keyword::If, "if"),
            std::make_pair(Keyword::Else, "else"),
            std::make_pair(Keyword::Return, "return"),
            std::make_pair(Keyword::Volatile, "volatile")
        });
}