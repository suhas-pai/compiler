/*
 * Parse/ParseNumber.h
 * © suhas pai
 */

#pragma once

#include <string_view>
#include "Source/SourceLocation.h"

namespace AST {
    struct NumberLiteral;
}

namespace Parse {
    enum class NumberKind : uint8_t {
        SignedInteger,
        UnsignedInteger,

        // Not yet supported
        FloatingPoint,
    };

    enum class ParseNumberErrorKind : uint8_t {
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

    struct ParseNumberError {
        ParseNumberErrorKind Kind;
        union {
            uint64_t IndexOfInvalid;
        };
    };

    struct ParseNumberSuccessResult {
        NumberKind Kind;
        std::string_view Suffix;

        union {
            uint64_t UInt;
            int64_t SInt;
            double Float64;
        };
    };

    struct ParseNumberResult {
        ParseNumberError Error;
        ParseNumberSuccessResult Success;
    };

    [[nodiscard]] auto
    ParseNumber(std::string_view Text,
                ParseNumberOptions Options = ParseNumberOptions()) noexcept
        -> ParseNumberResult;

    [[nodiscard]] auto
    ParseNumberLiteral(
        SourceLocation Loc,
        std::string_view Text,
        ParseNumberOptions Options = ParseNumberOptions()) noexcept
            -> AST::NumberLiteral *;
}