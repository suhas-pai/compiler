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

    [[nodiscard]] constexpr auto text() const noexcept {
        return this->Text;
    }

    [[nodiscard]] constexpr auto index() const noexcept {
        return this->Index;
    }

    [[nodiscard]] constexpr auto finished() const noexcept {
        return this->index() == this->text().size();
    }

    [[nodiscard]] constexpr auto peek() const noexcept -> char {
        if (this->index() >= this->text().size()) {
            return '\0';
        }

        return this->text().at(this->index());
    }

    [[nodiscard]] constexpr auto consumeIf(const char C) noexcept -> bool {
        if (this->peek() == C) {
            this->consume();
            return true;
        }

        return false;
    }

    [[nodiscard]] constexpr auto prev() const noexcept -> char {
        if (this->index() == 0) {
            return '\0';
        }

        return this->text().at(this->index() - 1);
    }

    constexpr auto consume(const uint32_t Skip = 0) noexcept -> char {
        if (this->index() + Skip >= this->text().size()) {
            return '\0';
        }

        this->Index += Skip + 1;
        return this->text().at(this->index() - 1);
    }
};