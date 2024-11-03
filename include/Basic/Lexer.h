/*
 * Basic/Lexer.h
 */

#pragma once
#include <string_view>

struct Lexer {
protected:
    std::string_view Text;
    uint32_t Index = 0;
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
        if (Index >= Text.size()) {
            return '\0';
        }

        return Text.at(Index);
    }

    [[nodiscard]] constexpr auto consumeIf(const char C) noexcept -> bool {
        if (this->peek() == C) {
            this->consume();
            return true;
        }

        return false;
    }

    [[nodiscard]] constexpr auto prev() const noexcept -> char {
        if (Index >= Text.size()) {
            return '\0';
        }

        return Text.at(Index);
    }

    constexpr auto consume(const uint64_t Skip = 0) noexcept -> char {
        if (this->getIndex() + Skip >= Text.size()) {
            return '\0';
        }

        this->Index += Skip + 1;
        return Text.at(this->getIndex() - 1);
    }
};