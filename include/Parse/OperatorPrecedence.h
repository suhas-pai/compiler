/*
 * Parser/OperatorPrecedence.h
 */

#pragma once

#include "ADT/SmallArrayMap.h"
#include "Lex/Token.h"

namespace Parse {
    enum class Precedence {
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
        PointerToMember,
    };

    enum class OperatorAssoc {
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

    constexpr auto OperatorInfoMap =
        ADT::SmallArrayMap<Lex::TokenKind, OperatorInfo, 31>({
            std::make_pair(Lex::TokenKind::Comma,
                           OperatorInfo(Precedence::Comma,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::Equal,
                           OperatorInfo(Precedence::Assignment,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::StarEqual,
                           OperatorInfo(Precedence::Assignment,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::SlashEqual,
                           OperatorInfo(Precedence::Assignment,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::PercentEqual,
                           OperatorInfo(Precedence::Assignment,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::PlusEqual,
                           OperatorInfo(Precedence::Assignment,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::MinusEqual,
                           OperatorInfo(Precedence::Assignment,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::ShiftLeftEqual,
                           OperatorInfo(Precedence::Assignment,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::ShiftRightEqual,
                           OperatorInfo(Precedence::Assignment,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::AmpersandEqual,
                           OperatorInfo(Precedence::Assignment,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::CaretEqual,
                           OperatorInfo(Precedence::Assignment,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::PipeEqual,
                           OperatorInfo(Precedence::Assignment,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::QuestionMark,
                           OperatorInfo(Precedence::Conditional,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::DoublePipe,
                           OperatorInfo(Precedence::LogicalOr,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::DoubleAmpersand,
                           OperatorInfo(Precedence::LogicalAnd,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::Pipe,
                           OperatorInfo(Precedence::InclusiveOr,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::Caret,
                           OperatorInfo(Precedence::ExclusiveOr,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::DoubleEqual,
                           OperatorInfo(Precedence::Equality,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::NotEqual,
                           OperatorInfo(Precedence::Equality,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::GreaterThanOrEqual,
                           OperatorInfo(Precedence::Relational,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::LessThanOrEqual,
                           OperatorInfo(Precedence::Relational,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::LessThan,
                           OperatorInfo(Precedence::Relational,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::GreaterThan,
                           OperatorInfo(Precedence::Relational,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::ShiftLeft,
                           OperatorInfo(Precedence::Shift,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::ShiftRight,
                           OperatorInfo(Precedence::Shift,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::Plus,
                           OperatorInfo(Precedence::Additive,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::Minus,
                           OperatorInfo(Precedence::Additive,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::Star,
                           OperatorInfo(Precedence::Multiplicative,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::Slash,
                           OperatorInfo(Precedence::Multiplicative,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::Percent,
                           OperatorInfo(Precedence::Multiplicative,
                                        OperatorAssoc::Left)),
            std::make_pair(Lex::TokenKind::DoubleStar,
                           OperatorInfo(Precedence::Power,
                                        OperatorAssoc::Left)),
    });
}