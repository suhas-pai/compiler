/*
 * Source/SourceLocation.h
 * © suhas pai
 */

#pragma once
#include <cstdint>

struct SourceLocation {
    constexpr static auto RowLimit = (1 << 20) - 1;
    constexpr static auto ColumnLimit = (1 << 12) - 1;

    uint32_t Index = 0;
    uint32_t Row : 20 = 0;
    uint16_t Column : 12 = 0;

    constexpr static auto forLine(const uint32_t Line) noexcept {
        return SourceLocation {
            .Index = 0,
            .Row = Line,
            .Column = 0
        };
    }

    [[nodiscard]] constexpr static auto invalid() noexcept {
        return SourceLocation {
            .Index = UINT32_MAX,
            .Row = 0,
            .Column = 0
        };
    }

    [[nodiscard]]
    constexpr auto adding(const uint16_t Offset) const noexcept {
        return SourceLocation {
            .Index = this->Index + Offset,
            .Row = this->Row,
            .Column = static_cast<uint16_t>(this->Column + Offset)
        };
    }
};

struct SourceRange {
    SourceLocation Start;
    SourceLocation End;
};
