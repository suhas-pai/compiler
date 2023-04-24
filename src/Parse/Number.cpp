/*
 * Parse/Number.cpp
 */

#include "Parse/Number.h"
#include "Basic/Lexer.h"

namespace Parse {
    auto
    ParseNumber(const std::string_view Text,
                const ParseNumberOptions Options) noexcept
        -> ParseNumberResult
    {
        auto Result = ParseNumberResult();
        if (Text.empty()) {
            Result.Error = ParseNumberError::EmptyString;
            return Result;
        }

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
                if (!Options.AllowPosSign) {
                    Result.Error = ParseNumberError::NegativeNumber;
                    return Result;
                }

                Lexer.consume();
            }
        }

        auto Base = uint8_t(10);
        if (const auto LeadingZeroChar = Lexer.peek()) {
            if (LeadingZeroChar == '0') {
                Lexer.consume();
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
                        break;
                }
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

                Result.SInt *= Base;
                Result.SInt -= Digit;
            }
        } else {
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
                } else  if (Char >= 'A' && Char <= 'F') {
                    Digit = Char - 'A';
                } else {
                    Result.Error = ParseNumberError::UnrecognizedChar;
                    return Result;
                }

                if (Digit >= Base) {
                    Result.Error  = ParseNumberError::InvalidDigit;
                    return Result;
                }

                Result.UInt *= Base;
                Result.UInt += Digit;
            }
        }

        return Result;
    }
}