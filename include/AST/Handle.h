/*
 * AST/Handle.h
 * © suhas pai
 */

#pragma once
#include <cstdint>

namespace AST {
    struct Node;
    struct Handle {
    protected:
        uint32_t Value;
    public:
        constexpr explicit Handle(const uint32_t Value) noexcept
        : Value(Value) {}

        [[nodiscard]] constexpr auto getRaw() const noexcept {
            return Value;
        }
    };
}