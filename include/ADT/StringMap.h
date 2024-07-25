/*
 * ADT/StringMap.h
 */

#include <unordered_map>
#include "Basic/StringHash.h"

namespace ADT {
    template <typename T>
    using UnorderedStringMap =
        std::unordered_map<std::string, T, StringHash, std::equal_to<>>;

    template <typename T>
    using UnorderedStringViewMap =
        std::unordered_map<std::string_view, T, StringHash, std::equal_to<>>;
}