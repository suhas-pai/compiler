/*
 * Lex/Token.h
 * © suhas pai
 */

#pragma once

#include <cassert>
#include <string_view>

#include "ADT/SmallArrayMap.h"
#include "Source/SourceLocation.h"

#include "Keyword.h"
#include "TokenKind.h"

namespace Lex {
    struct Token {
        TokenKind Kind;
        SourceLocation Loc;
        SourceLocation End;

        [[nodiscard]] constexpr static auto eof() {
            return Token {
                .Kind = TokenKind::EOFToken,
                .Loc = SourceLocation::invalid(),
                .End = SourceLocation::invalid()
            };
        }

        [[nodiscard]] constexpr static auto invalid() {
            return Token {
                .Kind = TokenKind::Invalid,
                .Loc = SourceLocation::invalid(),
                .End = SourceLocation::invalid()
            };
        }

        [[nodiscard]] constexpr auto isEof() const noexcept {
            return this->Kind == TokenKind::EOFToken;
        }

        [[nodiscard]] constexpr auto isInvalid() const noexcept {
            return this->Kind == TokenKind::Invalid;
        }

        [[nodiscard]]
        constexpr auto getString(const std::string_view Text) const noexcept {
            return Text.substr(this->Loc.Index,
                               this->End.Index - this->Loc.Index);
        }
    };

    [[nodiscard]] constexpr auto
    TokenStringIsKeyword(const std::string_view TokenString,
                         const Keyword Keyword) noexcept
    {
        return TokenString == KeywordToLexemeMap[Keyword];
    }

    [[nodiscard]] constexpr
    auto KeywordTokenGetKeyword(const std::string_view TokenString) noexcept {
        const auto KeywordNone = Keyword::Let;
        switch (KeywordNone) {
        #define CHECK_KW(KW)                                                   \
            case Keyword::KW:                                                  \
                if (TokenString == KeywordToLexemeMap[Keyword::KW]) {          \
                    return Keyword::KW;                                        \
                }

            CHECK_KW(Let)
            [[fallthrough]];

            CHECK_KW(Mut)
            [[fallthrough]];

            CHECK_KW(Function);
            [[fallthrough]];

            CHECK_KW(If);
            [[fallthrough]];

            CHECK_KW(Else);
            [[fallthrough]];

            CHECK_KW(Return);
            [[fallthrough]];

            CHECK_KW(Volatile);
            [[fallthrough]];

            CHECK_KW(Struct)
            [[fallthrough]];

            CHECK_KW(Enum)
            [[fallthrough]];

            CHECK_KW(And)
            [[fallthrough]];

            CHECK_KW(Or)
            [[fallthrough]];

            CHECK_KW(For)
            [[fallthrough]];

            CHECK_KW(Inline)
            [[fallthrough]];

            CHECK_KW(Comptime)
            [[fallthrough]];

            CHECK_KW(Default)
            [[fallthrough]];

            CHECK_KW(In)
            [[fallthrough]];

            CHECK_KW(As)
            [[fallthrough]];

            CHECK_KW(Discardable)
            break;

        #undef CHECK_KW
        }

        __builtin_unreachable();
    }
}