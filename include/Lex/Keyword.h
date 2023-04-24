/*
 * Lex/Keyword.h
 */

#pragma once

#include <string>
#include "ADT/SmallMap.h"

namespace Lex {
    enum class Keyword {
        Let,
    };

    const auto KeywordToLexemeMap =
        ADT::SmallMap<Keyword, std::string_view, 1>({
            std::make_pair(Keyword::Let, "let")
        });
}