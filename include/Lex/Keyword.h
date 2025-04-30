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
        Class,
        Shape,
        Union,
        Interface,
        Impl,
        Enum,
        And,
        Or,
        For,
        While,
        Inline,
        Comptime,
        Default,
        In,
        As,
        Discardable,
    };

    const auto KeywordToLexemeMap =
        ADT::SmallArrayMap<Keyword, std::string_view, 24>({
            std::make_pair(Keyword::Let, "let"),
            std::make_pair(Keyword::Mut, "mut"),
            std::make_pair(Keyword::Function, "func"),
            std::make_pair(Keyword::If, "if"),
            std::make_pair(Keyword::Else, "else"),
            std::make_pair(Keyword::Return, "return"),
            std::make_pair(Keyword::Volatile, "volatile"),
            std::make_pair(Keyword::Struct, "struct"),
            std::make_pair(Keyword::Class, "class"),
            std::make_pair(Keyword::Shape, "shape"),
            std::make_pair(Keyword::Union, "union"),
            std::make_pair(Keyword::Interface, "interface"),
            std::make_pair(Keyword::Impl, "impl"),
            std::make_pair(Keyword::Enum, "enum"),
            std::make_pair(Keyword::And, "and"),
            std::make_pair(Keyword::Or, "or"),
            std::make_pair(Keyword::While, "for"),
            std::make_pair(Keyword::For, "for"),
            std::make_pair(Keyword::Inline, "inline"),
            std::make_pair(Keyword::Comptime, "comptime"),
            std::make_pair(Keyword::Default, "default"),
            std::make_pair(Keyword::In, "in"),
            std::make_pair(Keyword::As, "as"),
            std::make_pair(Keyword::Discardable, "discardable")
        });
}