/*
 * Parse/ParseNumber.cpp
 * © suhas pai
 */

#include "AST/NumberLiteral.h"
#include "Basic/Lexer.h"

namespace Parse {
    auto
    ParseNumber(const std::string_view Text,
                const ParseNumberOptions Options) noexcept -> ParseNumberResult
    {
        auto Result = ParseNumberResult();
        auto Lexer = ::Lexer(Text);

        Result.Success.Kind = NumberKind::UnsignedInteger;
        if (const auto SignChar = Lexer.peek()) {
            if (SignChar == '-') {
                if (Options.DontAllowNegativeNumbers) {
                    Result.Error.Kind = ParseNumberErrorKind::NegativeNumber;
                    return Result;
                }

                Lexer.consume();
                Result.Success.Kind = NumberKind::SignedInteger;
            } else if (SignChar == '+') {
                if (!Options.AllowPositiveSign) {
                    Result.Error.Kind = ParseNumberErrorKind::PositiveSign;
                    return Result;
                }

                Lexer.consume();
            }
        } else {
            Result.Error.Kind = ParseNumberErrorKind::EmptyString;
            return Result;
        }

        auto Base = uint8_t(10);
        if (Lexer.consumeIf('0')) {
            switch (Lexer.peek()) {
                case '\0':
                    if (Result.Success.Kind == NumberKind::SignedInteger) {
                        Result.Error.Kind = ParseNumberErrorKind::NegativeZero;
                    } else {
                        Result.Success.UInt = 0;
                    }

                    return Result;
                case '.':
                    Result.Error.Kind = ParseNumberErrorKind::FloatingPoint;
                    return Result;
                case 'b':
                case 'B':
                    Lexer.consume();
                    Base = 2;

                    break;
                case 'o':
                case 'O':
                    Lexer.consume();
                    Base = 8;

                    break;
                case 'x':
                case 'X':
                    Lexer.consume();
                    Base = 16;

                    break;
                default:
                    Result.Error.Kind = ParseNumberErrorKind::UnrecognizedBase;
                    Result.Error.IndexOfInvalid = Lexer.index();

                    return Result;
            }
        }

        if (Result.Success.Kind == NumberKind::SignedInteger) {
            for (auto Char = Lexer.consume(); Char != '\0';
                 Char = Lexer.consume())
            {
                if (Char == '.') {
                    Result.Error.Kind = ParseNumberErrorKind::FloatingPoint;
                    return Result;
                }

                if (Char == 's' || Char == 'u') {
                    Result.Success.Suffix = Text.substr(Lexer.index() - 1);
                    break;
                }

                auto Digit = uint8_t(0);
                if (Char >= '0' && Char <= '9') {
                    Digit = Char - '0';
                } else  if (Char >= 'a' && Char <= 'z') {
                    Digit = Char - 'a';
                } else  if (Char >= 'A' && Char <= 'Z') {
                    Digit = Char - 'A';
                } else {
                    Result.Error.Kind = ParseNumberErrorKind::UnrecognizedChar;
                    Result.Error.IndexOfInvalid = Lexer.index() - 1;

                    return Result;
                }

                if (Digit >= Base) {
                    Result.Error.Kind = ParseNumberErrorKind::InvalidDigit;
                    Result.Error.IndexOfInvalid = Lexer.index() - 1;

                    return Result;
                }

                auto &Success = Result.Success;
                if (__builtin_mul_overflow(Success.SInt, Base, &Success.SInt) ||
                    __builtin_add_overflow(Success.SInt, Digit, &Success.SInt))
                {
                    Result.Error.Kind = ParseNumberErrorKind::Overflow;
                    Result.Error.IndexOfInvalid = Lexer.index() - 1;

                    return Result;
                };
            }
        } else {
            for (auto Char = Lexer.consume(); Char != '\0';
                 Char = Lexer.consume())
            {
                if (Char == '.') {
                    Result.Error.Kind = ParseNumberErrorKind::FloatingPoint;
                    Result.Error.IndexOfInvalid = Lexer.index() - 1;

                    return Result;
                }

                if (Char == 's' || Char == 'u') {
                    Result.Success.Suffix = Text.substr(Lexer.index() - 1);
                    break;
                }

                auto Digit = uint8_t();
                if (Char >= '0' && Char <= '9') {
                    Digit = Char - '0';
                } else  if (Char >= 'a' && Char <= 'z') {
                    Digit = Char - 'a';
                } else  if (Char >= 'A' && Char <= 'Z') {
                    Digit = Char - 'A';
                } else {
                    Result.Error.Kind = ParseNumberErrorKind::UnrecognizedChar;
                    Result.Error.IndexOfInvalid = Lexer.index() - 1;

                    return Result;
                }

                if (Digit >= Base) {
                    Result.Error.Kind = ParseNumberErrorKind::InvalidDigit;
                    Result.Error.IndexOfInvalid = Lexer.index() - 1;

                    return Result;
                }

                auto &Success = Result.Success;
                if (__builtin_mul_overflow(Success.UInt, Base, &Success.UInt) ||
                    __builtin_add_overflow(Success.UInt, Digit, &Success.UInt))
                {
                    Result.Error.Kind = ParseNumberErrorKind::Overflow;
                    Result.Error.IndexOfInvalid = Lexer.index() - 1;

                    return Result;
                };
            }
        }

        return Result;
    }

    auto
    ParseNumberLiteral(
        const SourceLocation Loc,
        const std::string_view Text,
        const ParseNumberOptions Options) noexcept
            -> AST::NumberLiteral *
    {
        return new AST::NumberLiteral(Loc, ParseNumber(Text, Options));
    }
}