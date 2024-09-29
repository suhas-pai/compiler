/*
 * Basic/SourceLocation.h
 */

#pragma once
#include <cstdint>

struct SourceLocation {
    constexpr static auto RowLimit = (1 << 20) - 1;
    constexpr static auto ColumnLimit = (1 << 12) - 1;

    uint32_t Index = 0;
    uint32_t Row : 20;
    uint16_t Column : 12;

    constexpr static auto forLine(const uint32_t Line) noexcept {
        return SourceLocation {
            .Index = 0,
            .Row = Line,
            .Column = 0
        };
    }

    [[nodiscard]] constexpr static auto invalid() noexcept {
        return SourceLocation({ .Index = 0, .Row = 0, .Column = 0 });
    }
};

struct SourceRange {
    SourceLocation Start;
    SourceLocation End;
};
