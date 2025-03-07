/*
 * Parse/Operator.h
 */

#pragma once
#include <string_view>

#include "ADT/SmallArrayMap.h"
#include "Lex/Token.h"

namespace Parse {
    enum class BinaryOperator : uint8_t {
        Assignment,

        Add,
        Subtract,
        Multiply,
        Modulo,
        Divide,

        LogicalAnd,
        LogicalOr,
        BitwiseAnd,
        BitwiseOr,
        BitwiseXor,

        LeftShift,
        RightShift,

        AddAssign,
        SubtractAssign,
        MultiplyAssign,
        DivideAssign,
        ModuloAssign,
        AndAssign,
        OrAssign,
        XorAssign,
        LeftShiftAssign,
        RightShiftAssign,

        LessThan,
        GreaterThan,
        LessThanOrEqual,
        GreaterThanOrEqual,

        Equality,
        Inequality,

        Power,
        As,
    };

    enum class UnaryOperator {
        Negate,
        LogicalNot,
        BitwiseNot,
        Increment,
        Decrement,
        AddressOf,
        Spread,
    };

    constexpr auto
    LexTokenToBinaryOperator(const Lex::Token Token,
                             const std::string_view Text) noexcept
        -> std::optional<BinaryOperator>
    {
        if (Token.Kind == Lex::TokenKind::Keyword) {
            switch (Lex::KeywordTokenGetKeyword(Text)) {
                case Lex::Keyword::And:
                    return BinaryOperator::LogicalAnd;
                case Lex::Keyword::Or:
                    return BinaryOperator::LogicalOr;
                case Lex::Keyword::Let:
                case Lex::Keyword::Mut:
                case Lex::Keyword::Function:
                case Lex::Keyword::If:
                case Lex::Keyword::Else:
                case Lex::Keyword::Return:
                case Lex::Keyword::Volatile:
                case Lex::Keyword::Struct:
                case Lex::Keyword::Class:
                case Lex::Keyword::Interface:
                case Lex::Keyword::Impl:
                case Lex::Keyword::Enum:
                case Lex::Keyword::For:
                case Lex::Keyword::While:
                case Lex::Keyword::Inline:
                case Lex::Keyword::Comptime:
                case Lex::Keyword::Default:
                case Lex::Keyword::In:
                case Lex::Keyword::Discardable:
                    break;
                case Lex::Keyword::As:
                    return BinaryOperator::As;
            }

            return std::nullopt;
        }

        switch (Token.Kind) {
            case Lex::TokenKind::Equal:
                return BinaryOperator::Assignment;
            case Lex::TokenKind::Plus:
                return BinaryOperator::Add;
            case Lex::TokenKind::Minus:
                return BinaryOperator::Subtract;
            case Lex::TokenKind::Star:
                return BinaryOperator::Multiply;
            case Lex::TokenKind::Percent:
                return BinaryOperator::Modulo;
            case Lex::TokenKind::Slash:
                return BinaryOperator::Divide;
            case Lex::TokenKind::Caret:
                return BinaryOperator::BitwiseXor;
            case Lex::TokenKind::Ampersand:
                return BinaryOperator::BitwiseAnd;
            case Lex::TokenKind::Pipe:
                return BinaryOperator::BitwiseOr;
            case Lex::TokenKind::ShiftLeft:
                return BinaryOperator::LeftShift;
            case Lex::TokenKind::ShiftRight:
                return BinaryOperator::RightShift;
            case Lex::TokenKind::PlusEqual:
                return BinaryOperator::AddAssign;
            case Lex::TokenKind::MinusEqual:
                return BinaryOperator::SubtractAssign;
            case Lex::TokenKind::StarEqual:
                return BinaryOperator::MultiplyAssign;
            case Lex::TokenKind::SlashEqual:
                return BinaryOperator::DivideAssign;
            case Lex::TokenKind::PercentEqual:
                return BinaryOperator::ModuloAssign;
            case Lex::TokenKind::AmpersandEqual:
                return BinaryOperator::AndAssign;
            case Lex::TokenKind::PipeEqual:
                return BinaryOperator::OrAssign;
            case Lex::TokenKind::CaretEqual:
                return BinaryOperator::XorAssign;
            case Lex::TokenKind::ShiftLeftEqual:
                return BinaryOperator::LeftShiftAssign;
            case Lex::TokenKind::ShiftRightEqual:
                return BinaryOperator::RightShiftAssign;
            case Lex::TokenKind::LessThan:
                return BinaryOperator::LessThan;
            case Lex::TokenKind::GreaterThan:
                return BinaryOperator::GreaterThan;
            case Lex::TokenKind::LessThanOrEqual:
                return BinaryOperator::LessThanOrEqual;
            case Lex::TokenKind::GreaterThanOrEqual:
                return BinaryOperator::GreaterThanOrEqual;
            case Lex::TokenKind::DoubleAmpersand:
                return BinaryOperator::LogicalAnd;
            case Lex::TokenKind::DoublePipe:
                return BinaryOperator::LogicalOr;
            case Lex::TokenKind::DoubleEqual:
                return BinaryOperator::Equality;
            case Lex::TokenKind::NotEqual:
                return BinaryOperator::Inequality;
            case Lex::TokenKind::DoubleStar:
                return BinaryOperator::Power;
            case Lex::TokenKind::IntegerLiteral:
            case Lex::TokenKind::IntegerLiteralWithSuffix:
            case Lex::TokenKind::FloatLiteral:
            case Lex::TokenKind::FloatLiteralWithSuffix:
            case Lex::TokenKind::CharLiteral:
            case Lex::TokenKind::StringLiteral:
            case Lex::TokenKind::Identifier:
            case Lex::TokenKind::Keyword:
            case Lex::TokenKind::Tilde:
            case Lex::TokenKind::TildeEqual:
            case Lex::TokenKind::Exclamation:
            case Lex::TokenKind::QuestionMark:
            case Lex::TokenKind::QuestionColon:
            case Lex::TokenKind::OpenParen:
            case Lex::TokenKind::CloseParen:
            case Lex::TokenKind::OpenCurlyBrace:
            case Lex::TokenKind::CloseCurlyBrace:
            case Lex::TokenKind::LeftSquareBracket:
            case Lex::TokenKind::RightSquareBracket:
            case Lex::TokenKind::Comma:
            case Lex::TokenKind::Colon:
            case Lex::TokenKind::Semicolon:
            case Lex::TokenKind::Dot:
            case Lex::TokenKind::DotStar:
            case Lex::TokenKind::DotDot:
            case Lex::TokenKind::DotDotLessThan:
            case Lex::TokenKind::DotDotGreaterThan:
            case Lex::TokenKind::DotDotEqual:
            case Lex::TokenKind::DotDotDot:
            case Lex::TokenKind::DotIdentifier:
            case Lex::TokenKind::ThinArrow:
            case Lex::TokenKind::FatArrow:
            case Lex::TokenKind::EOFToken:
            case Lex::TokenKind::Invalid:
                break;
        }

        return std::nullopt;
    }

    constexpr auto BinaryOperatorToLexemeMap =
        ADT::SmallArrayMap<BinaryOperator, std::string_view, 30>({
            std::make_pair(BinaryOperator::Assignment, "="),
            std::make_pair(BinaryOperator::Add, "+"),
            std::make_pair(BinaryOperator::Subtract, "-"),
            std::make_pair(BinaryOperator::Multiply, "*"),
            std::make_pair(BinaryOperator::Modulo, "%"),
            std::make_pair(BinaryOperator::Divide, "/"),
            std::make_pair(BinaryOperator::BitwiseXor, "^"),
            std::make_pair(BinaryOperator::BitwiseAnd, "&"),
            std::make_pair(BinaryOperator::BitwiseOr, "|"),
            std::make_pair(BinaryOperator::LeftShift, "<<"),
            std::make_pair(BinaryOperator::RightShift, ">>"),
            std::make_pair(BinaryOperator::AddAssign, "+="),
            std::make_pair(BinaryOperator::SubtractAssign, "-="),
            std::make_pair(BinaryOperator::MultiplyAssign, "*="),
            std::make_pair(BinaryOperator::DivideAssign, "/="),
            std::make_pair(BinaryOperator::ModuloAssign, "%="),
            std::make_pair(BinaryOperator::AndAssign, "&="),
            std::make_pair(BinaryOperator::OrAssign, "|="),
            std::make_pair(BinaryOperator::XorAssign, "^="),
            std::make_pair(BinaryOperator::LeftShiftAssign, "<<="),
            std::make_pair(BinaryOperator::RightShiftAssign, ">>="),
            std::make_pair(BinaryOperator::LogicalAnd, "and"),
            std::make_pair(BinaryOperator::LogicalOr, "or"),
            std::make_pair(BinaryOperator::LessThan, "<"),
            std::make_pair(BinaryOperator::GreaterThan, ">"),
            std::make_pair(BinaryOperator::LessThanOrEqual, "<="),
            std::make_pair(BinaryOperator::GreaterThanOrEqual, ">="),
            std::make_pair(BinaryOperator::Equality, "=="),
            std::make_pair(BinaryOperator::Inequality, "!="),
            std::make_pair(BinaryOperator::Power, "**"),
        });

    constexpr auto UnaryOperatorToLexemeMap =
        ADT::SmallArrayMap<UnaryOperator, std::string_view, 8>({
            std::make_pair(UnaryOperator::Negate, "-"),
            std::make_pair(UnaryOperator::LogicalNot, "!"),
            std::make_pair(UnaryOperator::BitwiseNot, "~"),
            std::make_pair(UnaryOperator::Increment, "++"),
            std::make_pair(UnaryOperator::Decrement, "--"),
            std::make_pair(UnaryOperator::AddressOf, "&"),
            std::make_pair(UnaryOperator::Spread, "..."),
        });
}