/*
 * Parse/String.cpp
 */

#include "Parse/String.h"
#include <optional>

namespace Parse {
    auto VerifyEscapeSequence(const char NextChar) -> char {
        switch (NextChar) {
            case 'a':
                return '\a';
            case 'b':
                return '\b';
            case 'f':
                return '\f';
            case 'n':
                return '\n';
            case 'r':
                return '\r';
            case 't':
                return '\t';
            case 'v':
                return '\v';
            case '\\':
                return '\\';
            case '\'':
                return '\'';
            case '"':
                return '"';
        }

        return '\0';
    }
}