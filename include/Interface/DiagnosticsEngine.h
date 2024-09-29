/*
 * Interface/DiagnosticsEngine.h
 */

#pragma once

#include <sys/cdefs.h>
#include <stdio.h>

#include "Basic/Macros.h"
#include "Basic/SourceLocation.h"

struct SourceManager;
namespace Interface {
    struct DiagnosticsEngine {
    protected:
        SourceManager *SrcMngr = nullptr;
        FILE *ErrorStream = nullptr;
    public:
        explicit DiagnosticsEngine() noexcept : ErrorStream(stderr) {}
        explicit DiagnosticsEngine(FILE *Stream) noexcept
        : ErrorStream(Stream) {}

        [[nodiscard]] static auto null() noexcept {
            return DiagnosticsEngine(nullptr);
        }

        constexpr auto setSourceManager(SourceManager *const SrcMngr) noexcept
            -> decltype(*this)
        {
            this->SrcMngr = SrcMngr;
            return *this;
        }

        __printflike(3, 4)
        void emitError(SourceLocation Loc, const char *Message, ...) noexcept;

        __printflike(3, 4)
        void emitWarning(SourceLocation Loc, const char *Message, ...) noexcept;

        __printflike(2, 3)
        void emitInternalError(const char *Message, ...) noexcept;
    };
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#define DIAG_ASSERT(diag, cond, msg, ...) \
    do { \
        if (__builtin_expect(!(cond), 0)) { \
            (diag).emitInternalError((msg), ##__VA_ARGS__); \
            exit(1); \
        } \
    } while (false)

#pragma clang diagnostic pop
