/*
 * Basic/SourceLocation.h
 */

#pragma once
#include <cstdint>

struct SourceLocation {
    int32_t Value = 0;

    [[nodiscard]] static constexpr auto invalid() noexcept {
        return SourceLocation({ .Value = -1 });
    }
};