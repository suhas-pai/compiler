/*
 * Parser/OperatorPrecedence.h
 */

#pragma once
#include "Lex/Token.h"

namespace Parse {
    enum class Precedence : uint8_t {
        Unknown,         // Not binary operator.
        Comma,           // ,
        Assignment,      // =, *=, /=, %=, +=, -=, <<=, >>=, &=, ^=, |=
        Conditional,     // ?
        LogicalOr,       // ||
        LogicalAnd,      // &&
        InclusiveOr,     // |
        ExclusiveOr,     // ^
        And,             // &
        Equality,        // ==, !=
        Relational,      // >=, <=, >, <
        Shift,           // <<, >>
        Additive,        // -, +
        Multiplicative,  // *, /, %
        Power,           // **
        PointerToMember, // ->
        Ternary,         // ?:
    };

    enum class OperatorAssoc : uint8_t {
        Left,
        Right
    };

    struct OperatorInfo {
        Precedence Precedence;
        OperatorAssoc Assoc;

        constexpr explicit
        OperatorInfo(const enum Precedence Precedence,
                     const OperatorAssoc Assoc) noexcept
        : Precedence(Precedence), Assoc(Assoc) {}
    };

    constexpr auto
    OperatorInfoForToken(const Lex::Token Token,
                         const std::string_view Text) noexcept
        -> OperatorInfo
    {
        switch (Token.Kind) {
            case Lex::TokenKind::Comma:
            case Lex::TokenKind::Equal:
            case Lex::TokenKind::StarEqual:
            case Lex::TokenKind::SlashEqual:
            case Lex::TokenKind::PercentEqual:
            case Lex::TokenKind::PlusEqual:
            case Lex::TokenKind::MinusEqual:
            case Lex::TokenKind::ShiftLeftEqual:
            case Lex::TokenKind::ShiftRightEqual:
            case Lex::TokenKind::AmpersandEqual:
            case Lex::TokenKind::CaretEqual:
            case Lex::TokenKind::PipeEqual:
                return OperatorInfo(Precedence::Assignment,
                                    OperatorAssoc::Left);
            case Lex::TokenKind::QuestionMark:
                return OperatorInfo(Precedence::Conditional,
                                    OperatorAssoc::Left);
            case Lex::TokenKind::QuestionColon:
                return OperatorInfo(Precedence::Ternary, OperatorAssoc::Left);
            case Lex::TokenKind::Keyword:
                switch (Lex::KeywordTokenGetKeyword(Text)) {
                    case Lex::Keyword::If:
                    case Lex::Keyword::Else:
                        return OperatorInfo(Precedence::Conditional,
                                            OperatorAssoc::Left);
                    case Lex::Keyword::For:
                        return OperatorInfo(Precedence::Comma,
                                            OperatorAssoc::Left);
                    case Lex::Keyword::While:
                        return OperatorInfo(Precedence::Comma,
                                            OperatorAssoc::Left);
                    case Lex::Keyword::And:
                        return OperatorInfo(Precedence::LogicalAnd,
                                            OperatorAssoc::Left);
                    case Lex::Keyword::Or:
                        return OperatorInfo(Precedence::LogicalOr,
                                            OperatorAssoc::Left);
                    case Lex::Keyword::Let:
                    case Lex::Keyword::Mut:
                    case Lex::Keyword::Function:
                    case Lex::Keyword::Return:
                    case Lex::Keyword::Volatile:
                    case Lex::Keyword::Struct:
                    case Lex::Keyword::Enum:
                    case Lex::Keyword::Inline:
                    case Lex::Keyword::Comptime:
                    case Lex::Keyword::Class:
                    case Lex::Keyword::Interface:
                    case Lex::Keyword::Impl:
                    case Lex::Keyword::Default:
                    case Lex::Keyword::In:
                    case Lex::Keyword::As:
                    case Lex::Keyword::Discardable:
                        break;
                }

                break;
            case Lex::TokenKind::Pipe:
                return OperatorInfo(Precedence::InclusiveOr,
                                    OperatorAssoc::Left);
            case Lex::TokenKind::Caret:
                return OperatorInfo(Precedence::ExclusiveOr,
                                    OperatorAssoc::Left);
            case Lex::TokenKind::DoubleEqual:
            case Lex::TokenKind::NotEqual:
            case Lex::TokenKind::GreaterThanOrEqual:
            case Lex::TokenKind::LessThanOrEqual:
            case Lex::TokenKind::LessThan:
            case Lex::TokenKind::GreaterThan:
                return OperatorInfo(Precedence::Relational,
                                    OperatorAssoc::Left);
            case Lex::TokenKind::ShiftLeft:
            case Lex::TokenKind::ShiftRight:
                return OperatorInfo(Precedence::Shift, OperatorAssoc::Left);
            case Lex::TokenKind::Plus:
            case Lex::TokenKind::Minus:
                return OperatorInfo(Precedence::Additive, OperatorAssoc::Left);
            case Lex::TokenKind::Star:
            case Lex::TokenKind::Slash:
            case Lex::TokenKind::Percent:
                return OperatorInfo(Precedence::Multiplicative,
                                    OperatorAssoc::Left);
            case Lex::TokenKind::DoubleStar:
                return OperatorInfo(Precedence::Power, OperatorAssoc::Left);
            case Lex::TokenKind::ThinArrow:
                return OperatorInfo(Precedence::PointerToMember,
                                    OperatorAssoc::Left);
            case Lex::TokenKind::Identifier:
            case Lex::TokenKind::IntegerLiteral:
            case Lex::TokenKind::IntegerLiteralWithSuffix:
            case Lex::TokenKind::FloatLiteral:
            case Lex::TokenKind::CharLiteral:
            case Lex::TokenKind::StringLiteral:
                return OperatorInfo(Precedence::Unknown, OperatorAssoc::Left);
            case Lex::TokenKind::Ampersand:
                return OperatorInfo(Precedence::And, OperatorAssoc::Left);
            case Lex::TokenKind::DoubleAmpersand:
                return OperatorInfo(Precedence::LogicalAnd,
                                    OperatorAssoc::Left);
            case Lex::TokenKind::DoublePipe:
                return OperatorInfo(Precedence::LogicalOr,
                                    OperatorAssoc::Left);
            case Lex::TokenKind::Tilde:
                return OperatorInfo(Precedence::Comma, OperatorAssoc::Left);
            case Lex::TokenKind::Colon:
                return OperatorInfo(Precedence::Ternary, OperatorAssoc::Left);
            case Lex::TokenKind::TildeEqual:
            case Lex::TokenKind::Exclamation:
            case Lex::TokenKind::OpenParen:
            case Lex::TokenKind::CloseParen:
            case Lex::TokenKind::OpenCurlyBrace:
            case Lex::TokenKind::CloseCurlyBrace:
            case Lex::TokenKind::LeftSquareBracket:
            case Lex::TokenKind::RightSquareBracket:
            case Lex::TokenKind::Semicolon:
            case Lex::TokenKind::Dot:
            case Lex::TokenKind::DotIdentifier:
            case Lex::TokenKind::DotStar:
            case Lex::TokenKind::DotDot:
            case Lex::TokenKind::DotDotLessThan:
            case Lex::TokenKind::DotDotGreaterThan:
            case Lex::TokenKind::DotDotEqual:
            case Lex::TokenKind::DotDotDot:
            case Lex::TokenKind::FatArrow:
            case Lex::TokenKind::EOFToken:
            case Lex::TokenKind::Invalid:
            case Lex::TokenKind::FloatLiteralWithSuffix:
                break;
        }

        return OperatorInfo(Precedence::Unknown, OperatorAssoc::Left);
    }
}