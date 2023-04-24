/*
 * Parse/String.h
 */

#pragma once
#include <optional>

namespace Parse {
    auto VerifyEscapeSequence(const char NextChar) -> char;
}