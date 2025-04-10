/*
 * ADT/SmallArrayMap.h
 */

#pragma once

#include <algorithm>
#include <array>
#include <optional>

namespace ADT {
    template <typename K, typename V, std::size_t Size>
    struct SmallArrayMap {
    protected:
        std::array<std::pair<K, V>, Size> Data;
    public:
        constexpr SmallArrayMap() noexcept = default;

        constexpr
        SmallArrayMap(const std::array<std::pair<K, V>, Size> &List) noexcept
        : Data(List) {}

        constexpr SmallArrayMap(const SmallArrayMap &Other) noexcept
        : Data(Other.Data) {}

        constexpr SmallArrayMap(SmallArrayMap &&Other) noexcept
        : Data(std::move(Other.Data)) {}

        [[nodiscard]]
        constexpr auto at(const K &Key) const noexcept -> std::optional<V> {
            const auto Iter =
                std::ranges::find_if(Data,
                                     [&Key](const auto &Pair) noexcept {
                                         return Pair.first == Key;
                                     });

            if (Iter == end(Data)) {
                return std::nullopt;
            }

            return Iter->second;
        }

        [[nodiscard]]
        constexpr auto keyFor(const V &Val) const noexcept -> std::optional<K> {
            const auto Iter =
                std::ranges::find_if(Data,
                                     [&Val](const auto &Pair) noexcept {
                                         return Pair.second == Val;
                                     });

            if (Iter == end(Data)) {
                return std::nullopt;
            }

            return Iter->first;
        }

        [[nodiscard]] constexpr auto operator[](const K &Key) const noexcept {
            return this->at(Key);
        }
    };
}