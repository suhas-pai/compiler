/*
 * Lex/Token.h
 * © suhas pai
 */

#pragma once

#include <cassert>
#include <string_view>

#include "Basic/SourceLocation.h"
#include "Keyword.h"

namespace Lex {
    enum class TokenKind : uint8_t {
        NumberLiteral,
        FloatLiteral,

        CharLiteral,
        StringLiteral,

        Identifier,
        Keyword,

        Plus,
        Minus,

        PlusEqual,
        MinusEqual,

        Star,
        DoubleStar,
        StarEqual,

        Slash,
        SlashEqual,

        Percent,
        PercentEqual,

        Shl,
        ShlEqual,

        Shr,
        ShrEqual,

        Caret,
        CaretEqual,

        Ampersand,
        AmpersandEqual,
        DoubleAmpersand,

        Pipe,
        PipeEqual,
        DoublePipe,

        Tilde,
        TildeEqual,

        LessThan,
        GreaterThan,

        LessThanOrEqual,
        GreaterThanOrEqual,
        Equal,

        Exclamation,
        NotEqual,
        DoubleEqual,

        QuestionMark,

        OpenParen,
        CloseParen,
        LeftCurlyBrace,
        RightCurlyBrace,
        LeftSquareBracket,

        RightSquareBracket,
        Comma,
        Colon,

        Semicolon,
        Dot,

        EOFToken,
        Invalid,
    };

    [[nodiscard]]
    constexpr auto TokenKindIsUnaryOp(const TokenKind Kind) noexcept -> bool {
        switch (Kind) {
            case TokenKind::Exclamation:
            case TokenKind::Tilde:
            case TokenKind::Ampersand:
            case TokenKind::Star:
                return true;
            case TokenKind::Plus:
            case TokenKind::Minus:
            case TokenKind::Slash:
            case TokenKind::DoubleStar:
            case TokenKind::Percent:
            case TokenKind::Shl:
            case TokenKind::Shr:
            case TokenKind::Caret:
            case TokenKind::Pipe:
            case TokenKind::LessThan:
            case TokenKind::GreaterThan:
            case TokenKind::LessThanOrEqual:
            case TokenKind::GreaterThanOrEqual:
            case TokenKind::Equal:
            case TokenKind::NotEqual:
            case TokenKind::NumberLiteral:
            case TokenKind::FloatLiteral:
            case TokenKind::CharLiteral:
            case TokenKind::StringLiteral:
            case TokenKind::Identifier:
            case TokenKind::Keyword:
            case TokenKind::PlusEqual:
            case TokenKind::MinusEqual:
            case TokenKind::StarEqual:
            case TokenKind::SlashEqual:
            case TokenKind::PercentEqual:
            case TokenKind::ShlEqual:
            case TokenKind::ShrEqual:
            case TokenKind::CaretEqual:
            case TokenKind::AmpersandEqual:
            case TokenKind::DoubleAmpersand:
            case TokenKind::PipeEqual:
            case TokenKind::DoublePipe:
            case TokenKind::TildeEqual:
            case TokenKind::DoubleEqual:
            case TokenKind::QuestionMark:
            case TokenKind::OpenParen:
            case TokenKind::CloseParen:
            case TokenKind::LeftCurlyBrace:
            case TokenKind::RightCurlyBrace:
            case TokenKind::LeftSquareBracket:
            case TokenKind::RightSquareBracket:
            case TokenKind::Comma:
            case TokenKind::Colon:
            case TokenKind::Semicolon:
            case TokenKind::Dot:
            case TokenKind::EOFToken:
                break;
            case TokenKind::Invalid:
                assert(false && "TokenKindIsUnaryOp() is invalid");
        }

        return false;
    }

    [[nodiscard]]
    constexpr auto TokenKindIsBinOp(const TokenKind Kind) noexcept -> bool {
        switch (Kind) {
            case TokenKind::Plus:
            case TokenKind::Minus:
            case TokenKind::Star:
            case TokenKind::Slash:
            case TokenKind::DoubleStar:
            case TokenKind::Percent:
            case TokenKind::Shl:
            case TokenKind::Shr:
            case TokenKind::Caret:
            case TokenKind::Ampersand:
            case TokenKind::Pipe:
            case TokenKind::LessThan:
            case TokenKind::GreaterThan:
            case TokenKind::LessThanOrEqual:
            case TokenKind::GreaterThanOrEqual:
            case TokenKind::Equal:
            case TokenKind::NotEqual:
                return true;
            case TokenKind::NumberLiteral:
            case TokenKind::FloatLiteral:
            case TokenKind::CharLiteral:
            case TokenKind::StringLiteral:
            case TokenKind::Identifier:
            case TokenKind::Keyword:
            case TokenKind::PlusEqual:
            case TokenKind::MinusEqual:
            case TokenKind::StarEqual:
            case TokenKind::SlashEqual:
            case TokenKind::PercentEqual:
            case TokenKind::ShlEqual:
            case TokenKind::ShrEqual:
            case TokenKind::CaretEqual:
            case TokenKind::AmpersandEqual:
            case TokenKind::DoubleAmpersand:
            case TokenKind::PipeEqual:
            case TokenKind::DoublePipe:
            case TokenKind::Tilde:
            case TokenKind::TildeEqual:
            case TokenKind::Exclamation:
            case TokenKind::DoubleEqual:
            case TokenKind::QuestionMark:
            case TokenKind::OpenParen:
            case TokenKind::CloseParen:
            case TokenKind::LeftCurlyBrace:
            case TokenKind::RightCurlyBrace:
            case TokenKind::LeftSquareBracket:
            case TokenKind::RightSquareBracket:
            case TokenKind::Comma:
            case TokenKind::Colon:
            case TokenKind::Semicolon:
            case TokenKind::Dot:
            case TokenKind::EOFToken:
                break;
            case TokenKind::Invalid:
                assert(false && "TokenKindIsBinOp() is invalid");
        }

        return false;
    }

    [[nodiscard]]
    constexpr auto TokenKindGetName(const TokenKind Kind) noexcept
        -> std::string_view
    {
        switch (Kind) {
            case TokenKind::NumberLiteral:
                return "number-literal";
            case TokenKind::FloatLiteral:
                return "float-literal";
            case TokenKind::CharLiteral:
                return "char-literal";
            case TokenKind::StringLiteral:
                return "string-literal";
            case TokenKind::Identifier:
                return "identifier";
            case TokenKind::Keyword:
                return "keyword";
            case TokenKind::Plus:
                return "plus-sign";
            case TokenKind::Minus:
                return "minus-sign";
            case TokenKind::PlusEqual:
                return "plus-equal";
            case TokenKind::MinusEqual:
                return "minus-equal";
            case TokenKind::Star:
                return "star-operator";
            case TokenKind::StarEqual:
                return "star-equal";
            case TokenKind::Slash:
                return "slash";
            case TokenKind::SlashEqual:
                return "slash-equql";
            case TokenKind::DoubleStar:
                return "two-star";
            case TokenKind::Percent:
                return "percent";
            case TokenKind::PercentEqual:
                return "percent-equal";
            case TokenKind::Shl:
                return "shl-operator";
            case TokenKind::ShlEqual:
                return "shl-equal";
            case TokenKind::Shr:
                return "shr-operator";
            case TokenKind::ShrEqual:
                return "shr-equal";
            case TokenKind::Caret:
                return "caret";
            case TokenKind::CaretEqual:
                return "caret-equal";
            case TokenKind::Ampersand:
                return "ampersand";
            case TokenKind::AmpersandEqual:
                return "ampersand-equal";
            case TokenKind::DoubleAmpersand:
                return "double-ampersand";
            case TokenKind::Pipe:
                return "pipe";
            case TokenKind::PipeEqual:
                return "pipe-equal";
            case TokenKind::DoublePipe:
                return "double-pipe";
            case TokenKind::Tilde:
                return "tilde";
            case TokenKind::TildeEqual:
                return "tilde-equal";
            case TokenKind::LessThan:
                return "less-than";
            case TokenKind::GreaterThan:
                return "greater-than";
            case TokenKind::LessThanOrEqual:
                return "less-than-or-equal";
            case TokenKind::GreaterThanOrEqual:
                return "greater-than-or-equal";
            case TokenKind::Equal:
                return "equal-sign";
            case TokenKind::Exclamation:
                return "bang";
            case TokenKind::NotEqual:
                return "not-equal";
            case TokenKind::DoubleEqual:
                return "double-equal";
            case TokenKind::QuestionMark:
                return "question-mark";
            case TokenKind::OpenParen:
                return "left-paren";
            case TokenKind::CloseParen:
                return "right-paren";
            case TokenKind::LeftCurlyBrace:
                return "left-curly-brace";
            case TokenKind::RightCurlyBrace:
                return "right-curly-brace";
            case TokenKind::LeftSquareBracket:
                return "left-square-bracket";
            case TokenKind::RightSquareBracket:
                return "right-square-bracket";
            case TokenKind::Comma:
                return "comma";
            case TokenKind::Colon:
                return "colon";
            case TokenKind::Semicolon:
                return "semicolon";
            case TokenKind::Dot:
                return "dot";
            case TokenKind::EOFToken:
                return "eof";
            case TokenKind::Invalid:
                assert(false && "TokenKindGetName() is invalid");
        }
    }

    struct Token {
        TokenKind Kind;
        SourceLocation Loc;
        SourceLocation End;

        [[nodiscard]] static auto eof() -> Token {
            return Token { .Kind = TokenKind::EOFToken };
        }

        [[nodiscard]] static auto invalid() -> Token {
            return Token { .Kind = TokenKind::Invalid };
        }

        [[nodiscard]]
        constexpr auto getString(const std::string_view Text) const noexcept
            -> std::string_view
        {
            return Text.substr(Loc.Value, End.Value - Loc.Value);
        }
    };

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

            CHECK_KW(Function);
            break;

        #undef CHECK_KW
        }

        assert(false &&
               "KeywordTokenGetKeyword() got keyword-token with unrecognized "
               "keyword");
    }
}