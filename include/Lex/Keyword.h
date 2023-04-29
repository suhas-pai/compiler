/*
 * Lex/Keyword.h
 */

#pragma once

#include <string>
#include "ADT/SmallMap.h"

namespace Lex {
    enum class Keyword {
        Let,
        Function
    };

    const auto KeywordToLexemeMap =
        ADT::SmallMap<Keyword, std::string_view, 2>({
            std::make_pair(Keyword::Let, "let"),
            std::make_pair(Keyword::Function, "func")
        });
}