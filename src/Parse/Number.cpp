/*
 * Parse/Number.cpp
 */

#include "Basic/Lexer.h"
#include "Parse/Number.h"

namespace Parse {
    auto
    ParseNumber(const std::string_view Text,
                const ParseNumberOptions Options) noexcept -> ParseNumberResult
    {
        auto Result = ParseNumberResult();
        Result.Kind = NumberKind::UnsignedInteger;

        auto Lexer = ::Lexer(Text);
        if (const auto SignChar = Lexer.peek()) {
            if (SignChar == '-') {
                if (Options.DontAllowNegativeNumbers) {
                    Result.Error = ParseNumberError::NegativeNumber;
                    return Result;
                }

                Lexer.consume();
                Result.Kind = NumberKind::SignedInteger;
            } else if (SignChar == '+') {
                if (!Options.AllowPositiveSign) {
                    Result.Error = ParseNumberError::PositiveSign;
                    return Result;
                }

                Lexer.consume();
            }
        } else {
            Result.Error = ParseNumberError::EmptyString;
            return Result;
        }

        auto Base = uint8_t(10);
        if (Lexer.consumeIf('0')) {
            switch (Lexer.peek()) {
                case '\0':
                    if (Result.Kind == NumberKind::SignedInteger) {
                        Result.Error = ParseNumberError::NegativeZero;
                    } else {
                        Result.UInt = 0;
                    }

                    return Result;
                case '.':
                    Result.Error = ParseNumberError::FloatingPoint;
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
                    Result.Error = ParseNumberError::UnrecognizedBase;
                    return Result;
            }
        }

        if (Result.Kind == NumberKind::SignedInteger) {
            for (auto Char = Lexer.consume(); Char != '\0';
                 Char = Lexer.consume())
            {
                if (Char == '.') {
                    Result.Error = ParseNumberError::FloatingPoint;
                    return Result;
                }

                auto Digit = uint8_t(0);
                if (Char >= '0' && Char <= '9') {
                    Digit = Char - '0';
                } else  if (Char >= 'a' && Char <= 'f') {
                    Digit = Char - 'a';
                } else  if (Char >= 'A' && Char <= 'Z') {
                    Digit = Char - 'A';
                } else {
                    Result.Error = ParseNumberError::UnrecognizedChar;
                    return Result;
                }

                if (Digit >= Base) {
                    Result.Error  = ParseNumberError::InvalidDigit;
                    return Result;
                }

                if (__builtin_mul_overflow(Result.SInt, Base, &Result.SInt)
                 || __builtin_add_overflow(Result.SInt, Digit, &Result.SInt))
                {
                    Result.Error = ParseNumberError::Overflow;
                    return Result;
                };
            }
        } else {
            for (auto Char = Lexer.consume(); Char != '\0';
                 Char = Lexer.consume())
            {
                if (Char == '.') {
                    Result.Error = ParseNumberError::FloatingPoint;
                    return Result;
                }

                auto Digit = uint8_t();
                if (Char >= '0' && Char <= '9') {
                    Digit = Char - '0';
                } else  if (Char >= 'a' && Char <= 'f') {
                    Digit = Char - 'a';
                } else  if (Char >= 'A' && Char <= 'F') {
                    Digit = Char - 'A';
                } else {
                    Result.Error = ParseNumberError::UnrecognizedChar;
                    return Result;
                }

                if (Digit >= Base) {
                    Result.Error = ParseNumberError::InvalidDigit;
                    return Result;
                }

                if (__builtin_mul_overflow(Result.UInt, Base, &Result.UInt)
                 || __builtin_add_overflow(Result.UInt, Digit, &Result.UInt))
                {
                    Result.Error = ParseNumberError::Overflow;
                    return Result;
                };
            }
        }

        return Result;
    }
}