/*
 * include/Lex/ArgvLexer.h
 */

#pragma once

struct ArgvLexer {
protected:
    const char *const *Argv;

    // Start from 1 to skip program-name.
    int Argc;
    int Index = 1;
public:
    constexpr explicit
    ArgvLexer(const int Argc, const char *const *const Argv) noexcept
    : Argv(Argv), Argc(Argc) {}

    [[nodiscard]] constexpr auto peek() const noexcept {
        return Argv[Index];
    }

    [[nodiscard]] constexpr auto current() noexcept -> const char * {
        if (Index == 0) {
            return nullptr;
        }

        return Argv[Index - 1];
    }

    constexpr auto consume() noexcept -> const char * {
        if (Index == Argc) {
            return nullptr;
        }

        return Argv[Index++];
    }
};