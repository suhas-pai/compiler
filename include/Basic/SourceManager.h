/*
 * Basic/SourceManager.h
 */

#pragma once
#include <string>

struct SourceManager {
protected:
    std::string Text;
public:
    constexpr explicit SourceManager(std::string_view Text) noexcept
    : Text(Text) {}

    [[nodiscard]] constexpr auto getText() const noexcept -> std::string_view {
        return Text;
    }
};