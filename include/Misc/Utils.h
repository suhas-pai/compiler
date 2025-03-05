/*
 * Misc/Utils.h
 * © suhas pai
 */

#pragma once

#include <cctype>
#include <cstdint>

namespace Misc {
    constexpr auto hexdigit_to_number(const char Ch) noexcept -> uint8_t {
        return std::isdigit(Ch) ? Ch - '0' : 10 + (Ch | 32) - 'a';
    }
}
