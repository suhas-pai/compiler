/*
 * Basic/Lexer.h
 */

#pragma once
#include <string_view>

struct Lexer {
protected:
    std::string_view Text;
    uint64_t Index = 0;
public:
    constexpr explicit Lexer(const std::string_view Text) noexcept
    : Text(Text) {}

    [[nodiscard]] constexpr auto getText() const noexcept {
        return Text;
    }

    [[nodiscard]] constexpr auto getIndex() const noexcept {
        return Index;
    }

    [[nodiscard]] constexpr auto peek() const noexcept -> char {
        const auto Index = getIndex();
        if (Index == 0) {
            return '\0';
        }

        return Text.at(Index);
    }

    [[nodiscard]] constexpr auto prev() const noexcept -> char {
        const auto Index = getIndex();
        if (Index >= Text.size()) {
            return '\0';
        }

        return Text.at(Index);
    }

    constexpr auto consume(const uint64_t Skip = 0) noexcept -> char {
        if (getIndex() + Skip >= Text.size()) {
            return '\0';
        }

        this->Index += Skip + 1;
        return Text.at(getIndex() - 1);
    }
};