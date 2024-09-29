/*
 * Parse/Operator.h
 */

#pragma once
#include <string_view>

#include "ADT/SmallArrayMap.h"
#include "Lex/Token.h"

namespace Parse {
    enum class BinaryOperator {
        Assignment,

        Add,
        Subtract,

        Multiply,
        Modulo,
        Divide,

        Equality,
        Inequality,

        Power,
    };

    enum class UnaryOperator {
        Negate,
        LogicalNot,
        BitwiseNot,
        Increment,
        Decrement,
        AddressOf,
        Dereference,
    };

    constexpr auto LexTokenKindToBinaryOperatorMap =
        ADT::SmallArrayMap<Lex::TokenKind, BinaryOperator, 9>({
            std::make_pair(Lex::TokenKind::Equal, BinaryOperator::Assignment),
            std::make_pair(Lex::TokenKind::Plus, BinaryOperator::Add),
            std::make_pair(Lex::TokenKind::Minus, BinaryOperator::Subtract),
            std::make_pair(Lex::TokenKind::Star, BinaryOperator::Multiply),
            std::make_pair(Lex::TokenKind::Percent, BinaryOperator::Modulo),
            std::make_pair(Lex::TokenKind::Slash, BinaryOperator::Divide),
            std::make_pair(Lex::TokenKind::DoubleEqual,
                           BinaryOperator::Equality),
            std::make_pair(Lex::TokenKind::NotEqual,
                           BinaryOperator::Inequality),
            std::make_pair(Lex::TokenKind::DoubleStar, BinaryOperator::Power),
        });

    constexpr auto BinaryOperatorToLexemeMap =
        ADT::SmallArrayMap<BinaryOperator, std::string_view, 9>({
            std::make_pair(BinaryOperator::Assignment, "="),
            std::make_pair(BinaryOperator::Add, "+"),
            std::make_pair(BinaryOperator::Subtract, "-"),
            std::make_pair(BinaryOperator::Multiply, "*"),
            std::make_pair(BinaryOperator::Modulo, "%"),
            std::make_pair(BinaryOperator::Divide, "/"),
            std::make_pair(BinaryOperator::Equality, "=="),
            std::make_pair(BinaryOperator::Inequality, "!="),
            std::make_pair(BinaryOperator::Power, "**"),
        });

    constexpr auto UnaryOperatorToLexemeMap =
        ADT::SmallArrayMap<UnaryOperator, std::string_view, 7>({
            std::make_pair(UnaryOperator::Negate, "-"),
            std::make_pair(UnaryOperator::LogicalNot, "!"),
            std::make_pair(UnaryOperator::BitwiseNot, "~"),
            std::make_pair(UnaryOperator::Increment, "++"),
            std::make_pair(UnaryOperator::Decrement, "--"),
            std::make_pair(UnaryOperator::AddressOf, "&"),
            std::make_pair(UnaryOperator::Dereference, "*"),
        });
}