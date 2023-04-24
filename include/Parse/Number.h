/*
 * Parse/Number.h
 */

#pragma once
#include <string_view>

namespace Parse {
    enum class NumberKind : uint8_t {
        SignedInteger,
        UnsignedInteger,

        /* Not yet supported */
        FloatingPoint32,
        FloatingPoint64,
    };

    struct ParseNumberOptions {
        bool AllowPosSign : 1 = false;
        bool DontAllowNegativeNumbers : 1 = false;
    };

    enum class ParseNumberError : uint8_t {
        None,
        EmptyString,

        NegativeNumber,
        PositiveSign,

        InvalidDigit,
        UnrecognizedChar,

        TooLarge,
        LeadingZero,
        NegativeZero,

        FloatingPoint
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