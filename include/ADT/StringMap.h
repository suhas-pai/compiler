/*
 * ADT/StringMap.h
 */

#include <unordered_map>
#include "Basic/StringHash.h"

namespace ADT {
    template <typename T>
    using UnorderedStringMap =
        std::unordered_map<std::string, T, StringHash, std::equal_to<>>;
}