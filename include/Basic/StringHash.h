/*
 * Basic/StringHash.h
 */

#include "llvm/ADT/StringRef.h"
#include <string>

struct StringHash {
    using hash_type = std::hash<std::string_view>;
    using is_transparent = void;

    [[nodiscard]]
    constexpr auto operator()(const char *const String) const noexcept {
        return hash_type{}(String);
    }

    [[nodiscard]]
    constexpr auto operator()(const std::string_view String) const noexcept {
        return hash_type{}(String);
    }

    [[nodiscard]]
    constexpr auto operator()(const llvm::StringRef &String) const noexcept {
        return this->operator()(std::string_view(String));
    }

    [[nodiscard]]
    constexpr auto operator()(const std::string &String) const noexcept {
        return hash_type{}(String);
    }
};