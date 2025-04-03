/*
 * Lex/TokenKind.h
 * © suhas pai
 */

#pragma once

#include <cassert>
#include <string_view>

#include "Lex/Keyword.h"

namespace Lex {
    enum class TokenKind : uint8_t {
        IntegerLiteral,
        IntegerLiteralWithSuffix,

        FloatLiteral,
        FloatLiteralWithSuffix,

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

        ShiftLeft,
        ShiftLeftEqual,

        ShiftRight,
        ShiftRightEqual,

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
        OpenCurlyBrace,
        CloseCurlyBrace,
        LeftSquareBracket,

        RightSquareBracket,
        Comma,
        Colon,

        Semicolon,

        Dot,
        DotIdentifier,
        DotStar,

        DotDot,
        DotDotLessThan,
        DotDotGreaterThan,
        DotDotEqual,

        DotDotDot,

        ThinArrow,
        FatArrow,

        EOFToken,
        Invalid,
    };

    [[nodiscard]]
    constexpr auto TokenKindIsUnaryOp(const TokenKind Kind) noexcept {
        switch (Kind) {
            case TokenKind::Exclamation:
            case TokenKind::Tilde:
            case TokenKind::Ampersand:
            case TokenKind::Star:
            case TokenKind::DotDotDot:
            case TokenKind::QuestionMark:
                return true;
            case TokenKind::Plus:
            case TokenKind::Minus:
            case TokenKind::Slash:
            case TokenKind::DoubleStar:
            case TokenKind::Percent:
            case TokenKind::ShiftLeft:
            case TokenKind::ShiftRight:
            case TokenKind::Caret:
            case TokenKind::Pipe:
            case TokenKind::LessThan:
            case TokenKind::GreaterThan:
            case TokenKind::LessThanOrEqual:
            case TokenKind::GreaterThanOrEqual:
            case TokenKind::Equal:
            case TokenKind::NotEqual:
            case TokenKind::IntegerLiteral:
            case TokenKind::IntegerLiteralWithSuffix:
            case TokenKind::FloatLiteral:
            case TokenKind::FloatLiteralWithSuffix:
            case TokenKind::CharLiteral:
            case TokenKind::StringLiteral:
            case TokenKind::Identifier:
            case TokenKind::Keyword:
            case TokenKind::PlusEqual:
            case TokenKind::MinusEqual:
            case TokenKind::StarEqual:
            case TokenKind::SlashEqual:
            case TokenKind::PercentEqual:
            case TokenKind::ShiftLeftEqual:
            case TokenKind::ShiftRightEqual:
            case TokenKind::CaretEqual:
            case TokenKind::AmpersandEqual:
            case TokenKind::DoubleAmpersand:
            case TokenKind::PipeEqual:
            case TokenKind::DoublePipe:
            case TokenKind::TildeEqual:
            case TokenKind::DoubleEqual:
            case TokenKind::OpenParen:
            case TokenKind::CloseParen:
            case TokenKind::OpenCurlyBrace:
            case TokenKind::CloseCurlyBrace:
            case TokenKind::LeftSquareBracket:
            case TokenKind::RightSquareBracket:
            case TokenKind::Comma:
            case TokenKind::Colon:
            case TokenKind::Semicolon:
            case TokenKind::Dot:
            case TokenKind::DotIdentifier:
            case TokenKind::DotStar:
            case TokenKind::DotDot:
            case TokenKind::DotDotLessThan:
            case TokenKind::DotDotGreaterThan:
            case TokenKind::DotDotEqual:
            case TokenKind::ThinArrow:
            case TokenKind::FatArrow:
            case TokenKind::EOFToken:
                break;
            case TokenKind::Invalid:
                assert(false && "TokenKindIsUnaryOp(): TokenKind is invalid");
        }

        return false;
    }

    [[nodiscard]] constexpr auto
    TokenKindIsBinOp(const TokenKind Kind, const std::string_view Text) noexcept
    {
        switch (Kind) {
            case TokenKind::Plus:
            case TokenKind::Minus:
            case TokenKind::PlusEqual:
            case TokenKind::MinusEqual:
            case TokenKind::StarEqual:
            case TokenKind::SlashEqual:
            case TokenKind::PercentEqual:
            case TokenKind::ShiftLeftEqual:
            case TokenKind::ShiftRightEqual:
            case TokenKind::CaretEqual:
            case TokenKind::AmpersandEqual:
            case TokenKind::DoubleAmpersand:
            case TokenKind::Star:
            case TokenKind::Slash:
            case TokenKind::DoubleStar:
            case TokenKind::Percent:
            case TokenKind::ShiftLeft:
            case TokenKind::ShiftRight:
            case TokenKind::Caret:
            case TokenKind::Ampersand:
            case TokenKind::Pipe:
            case TokenKind::LessThan:
            case TokenKind::GreaterThan:
            case TokenKind::LessThanOrEqual:
            case TokenKind::GreaterThanOrEqual:
            case TokenKind::DoubleEqual:
            case TokenKind::Equal:
            case TokenKind::NotEqual:
            case TokenKind::QuestionMark:
                return true;
            case TokenKind::IntegerLiteral:
            case TokenKind::IntegerLiteralWithSuffix:
            case TokenKind::FloatLiteral:
            case TokenKind::FloatLiteralWithSuffix:
            case TokenKind::CharLiteral:
            case TokenKind::StringLiteral:
            case TokenKind::Identifier:
                return false;
            case TokenKind::Keyword:
                if (Text == KeywordToLexemeMap[Keyword::As]) {
                    return true;
                }

                if (Text == KeywordToLexemeMap[Keyword::And]) {
                    return true;
                }

                if (Text == KeywordToLexemeMap[Keyword::Or]) {
                    return true;
                }

                [[fallthrough]];
            case TokenKind::PipeEqual:
            case TokenKind::DoublePipe:
            case TokenKind::Tilde:
            case TokenKind::TildeEqual:
            case TokenKind::Exclamation:
            case TokenKind::OpenParen:
            case TokenKind::CloseParen:
            case TokenKind::OpenCurlyBrace:
            case TokenKind::CloseCurlyBrace:
            case TokenKind::LeftSquareBracket:
            case TokenKind::RightSquareBracket:
            case TokenKind::Comma:
            case TokenKind::Colon:
            case TokenKind::Semicolon:
            case TokenKind::Dot:
            case TokenKind::DotIdentifier:
            case TokenKind::DotStar:
            case TokenKind::DotDot:
            case TokenKind::DotDotLessThan:
            case TokenKind::DotDotGreaterThan:
            case TokenKind::DotDotEqual:
            case TokenKind::DotDotDot:
            case TokenKind::ThinArrow:
            case TokenKind::FatArrow:
            case TokenKind::EOFToken:
                return false;
            case TokenKind::Invalid:
                assert(false && "TokenKindIsBinOp(): TokenKind is invalid");
        }

        __builtin_unreachable();
    }

    [[nodiscard]] constexpr auto TokenKindGetName(const TokenKind Kind) noexcept
        -> std::string_view
    {
        switch (Kind) {
            case TokenKind::IntegerLiteral:
                return "integer-literal";
            case TokenKind::IntegerLiteralWithSuffix:
                return "integer-literal-with-suffix";
            case TokenKind::FloatLiteral:
                return "float-literal";
            case TokenKind::FloatLiteralWithSuffix:
                return "float-literal-with-suffix";
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
                return "slash-equal";
            case TokenKind::DoubleStar:
                return "two-star";
            case TokenKind::Percent:
                return "percent";
            case TokenKind::PercentEqual:
                return "percent-equal";
            case TokenKind::ShiftLeft:
                return "shl-operator";
            case TokenKind::ShiftLeftEqual:
                return "shl-equal";
            case TokenKind::ShiftRight:
                return "shr-operator";
            case TokenKind::ShiftRightEqual:
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
            case TokenKind::OpenCurlyBrace:
                return "open-curly-brace";
            case TokenKind::CloseCurlyBrace:
                return "close-curly-brace";
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
            case TokenKind::DotIdentifier:
                return "dot-identifier";
            case TokenKind::DotStar:
                return "dot-star";
            case TokenKind::DotDot:
                return "dot-dot";
            case TokenKind::DotDotLessThan:
                return "dot-dot-less-than";
            case TokenKind::DotDotGreaterThan:
                return "dot-dot-greater-than";
            case TokenKind::DotDotEqual:
                return "dot-dot-equal";
            case TokenKind::DotDotDot:
                return "dot-dot-dot";
            case TokenKind::ThinArrow:
                return "thin-arrow";
            case TokenKind::FatArrow:
                return "fat-arrow";
            case TokenKind::EOFToken:
                return "eof";
            case TokenKind::Invalid:
                assert(false && "TokenKindGetName(): token is invalid");
        }

        __builtin_unreachable();
    }

    [[nodiscard]]
    constexpr auto TokenKindGetLexeme(const TokenKind Kind) noexcept
        -> std::optional<std::string_view>
    {
        switch (Kind) {
            case TokenKind::IntegerLiteral:
            case TokenKind::IntegerLiteralWithSuffix:
            case TokenKind::FloatLiteral:
            case TokenKind::FloatLiteralWithSuffix:
            case TokenKind::CharLiteral:
            case TokenKind::StringLiteral:
            case TokenKind::Identifier:
            case TokenKind::Keyword:
                return std::nullopt;
            case TokenKind::Plus:
                return "+";
            case TokenKind::Minus:
                return "-";
            case TokenKind::PlusEqual:
                return "+=";
            case TokenKind::MinusEqual:
                return "-=";
            case TokenKind::Star:
                return "*";
            case TokenKind::StarEqual:
                return "*=";
            case TokenKind::Slash:
                return "/";
            case TokenKind::SlashEqual:
                return "/=";
            case TokenKind::DoubleStar:
                return "**";
            case TokenKind::Percent:
                return "%";
            case TokenKind::PercentEqual:
                return "%=";
            case TokenKind::ShiftLeft:
                return "<<";
            case TokenKind::ShiftLeftEqual:
                return "<<=";
            case TokenKind::ShiftRight:
                return ">>";
            case TokenKind::ShiftRightEqual:
                return ">>=";
            case TokenKind::Caret:
                return "^";
            case TokenKind::CaretEqual:
                return "^=";
            case TokenKind::Ampersand:
                return "&";
            case TokenKind::AmpersandEqual:
                return "&=";
            case TokenKind::DoubleAmpersand:
                return "&&";
            case TokenKind::Pipe:
                return "|";
            case TokenKind::PipeEqual:
                return "|=";
            case TokenKind::DoublePipe:
                return "||";
            case TokenKind::Tilde:
                return "~";
            case TokenKind::TildeEqual:
                return "~=";
            case TokenKind::LessThan:
                return "<";
            case TokenKind::GreaterThan:
                return ">";
            case TokenKind::LessThanOrEqual:
                return "<=";
            case TokenKind::GreaterThanOrEqual:
                return ">=";
            case TokenKind::Equal:
                return "=";
            case TokenKind::Exclamation:
                return "!";
            case TokenKind::NotEqual:
                return "!=";
            case TokenKind::DoubleEqual:
                return "==";
            case TokenKind::QuestionMark:
                return "?";
            case TokenKind::OpenParen:
                return "(";
            case TokenKind::CloseParen:
                return ")";
            case TokenKind::OpenCurlyBrace:
                return "{";
            case TokenKind::CloseCurlyBrace:
                return "}";
            case TokenKind::LeftSquareBracket:
                return "[";
            case TokenKind::RightSquareBracket:
                return "]";
            case TokenKind::Comma:
                return ",";
            case TokenKind::Colon:
                return ":";
            case TokenKind::Semicolon:
                return ";";
            case TokenKind::Dot:
                return ".";
            case TokenKind::DotIdentifier:
                return std::nullopt;
            case TokenKind::DotStar:
                return ".*";
            case TokenKind::DotDot:
                return "..";
            case TokenKind::DotDotLessThan:
                return "..<";
            case TokenKind::DotDotGreaterThan:
                return "..>";
            case TokenKind::DotDotEqual:
                return "..=";
            case TokenKind::DotDotDot:
                return "...";
            case TokenKind::ThinArrow:
                return "->";
            case TokenKind::FatArrow:
                return "=>";
            case TokenKind::EOFToken:
                return std::nullopt;
            case TokenKind::Invalid:
                assert(false && "TokenKindGetLexeme(): token is invalid");
        }

        __builtin_unreachable();
    }
}
