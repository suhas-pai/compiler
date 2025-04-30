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
            const auto Length = this->End.Index - this->Loc.Index;
            return Text.substr(this->Loc.Index, Length);
        }
    };

    [[nodiscard]] constexpr auto
    TokenStringIsKeyword(const std::string_view TokenString,
                         const Keyword Keyword) noexcept
    {
        return TokenString == KeywordToLexemeMap[Keyword];
    }

    [[nodiscard]] constexpr
    auto KeywordLexemeGetKeyword(const std::string_view Lexeme) noexcept {
        const auto Keyword = KeywordToLexemeMap.keyFor(Lexeme);
        if (Keyword.has_value()) {
            return Keyword.value();
        }

        __builtin_unreachable();
    }
}
