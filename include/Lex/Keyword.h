/*
 * Lex/Keyword.h
 */

#pragma once

#include <string_view>
#include <utility>

#include "ADT/SmallMap.h"

namespace Lex {
    enum class Keyword {
        Let,
        Function,
        If,
        Else,
        Return
    };

    const auto KeywordToLexemeMap =
        ADT::SmallMap<Keyword, std::string_view, 5>({
            std::make_pair(Keyword::Let, "let"),
            std::make_pair(Keyword::Function, "func"),
            std::make_pair(Keyword::If, "if"),
            std::make_pair(Keyword::Else, "else"),
            std::make_pair(Keyword::Return, "return")
        });
}