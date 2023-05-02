/*
 * Basic/ErrorOr.h
 */

#pragma once

#include <concepts>
#include <functional>
#include <variant>

struct ErrorBase {};

template <typename T, typename E>
    requires std::is_pointer_v<T> && std::derived_from<E, ErrorBase>

struct ErrorOr {
protected:
    std::variant<T, E> Value;
public:
    constexpr ErrorOr(const T Value) noexcept : Value(Value) {}
    constexpr ErrorOr(const E &Value) noexcept : Value(Value) {}
    constexpr ErrorOr(const std::nullptr_t) noexcept = delete;

    [[nodiscard]] constexpr auto hasError() const noexcept {
        return std::holds_alternative<E>(Value);
    }

    [[nodiscard]] constexpr auto getError() const noexcept {
        return hasError() ? std::get<E>(Value) : nullptr;
    }
};