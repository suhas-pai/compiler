/*
 * ADT/Map.h
 */

#pragma once

#include <array>
#include <optional>

namespace ADT {
    template <typename K, typename V, std::size_t Size>
    struct SmallMap {
        std::array<std::pair<K, V>, Size> Data;

        [[nodiscard]]
        constexpr auto at(const K &Key) const noexcept -> std::optional<V> {
            const auto Iter =
                std::find_if(begin(Data),
                             end(Data),
                             [&Key](const auto &Pair) noexcept {
                                return Pair.first == Key;
                             });

            if (Iter == end(Data)) {
                return std::nullopt;
            }

            return Iter->second;
        }

        [[nodiscard]] constexpr
        auto keyFor(const V &Val) const noexcept -> std::optional<K> {
            const auto Iter =
                std::find_if(begin(Data),
                             end(Data),
                             [&Val](const auto &Pair) noexcept {
                                return Pair.second == Val;
                             });

            if (Iter == end(Data)) {
                return std::nullopt;
            }

            return Iter->first;
        }

        [[nodiscard]] constexpr auto operator[](const K &Key) const noexcept {
            return at(Key);
        }
    };
}