/*
 * Parse/Number.h
 */

#pragma once
#include <string_view>

namespace Parse {
    enum class NumberKind : uint8_t {
        SignedInteger,
        UnsignedInteger,

        // Not yet supported
        FloatingPoint8,
        FloatingPoint16,
        FloatingPoint32,
        FloatingPoint64,
    };

    enum class ParseNumberError : uint8_t {
        None,
        EmptyString,

        NegativeNumber,
        PositiveSign,

        InvalidDigit,
        UnrecognizedChar,
        UnrecognizedBase,

        TooLarge,
        LeadingZero,
        NegativeZero,

        FloatingPoint,
        Overflow,
    };

    struct ParseNumberOptions {
        bool AllowPositiveSign : 1 = false;
        bool DontAllowNegativeNumbers : 1 = false;
    };

    struct ParseNumberResult {
        NumberKind Kind;
        ParseNumberError Error;

        union {
            uint64_t UInt;
            int64_t SInt;
            float Float32;
            double Float64;
        };
    };

    [[nodiscard]] auto
    ParseNumber(std::string_view Text,
                ParseNumberOptions Options = ParseNumberOptions()) noexcept
        -> ParseNumberResult;
}