/*
 * Interface/DiagnosticsEngine.h
 */

#pragma once
#include <stdio.h>

#include <sys/cdefs.h>
#include "Basic/Macros.h"

namespace Interface {
    struct DiagnosticsEngine {
    protected:
        FILE *ErrorStream = NULL;
    public:
        explicit DiagnosticsEngine() noexcept : ErrorStream(stderr) {}
        explicit DiagnosticsEngine(FILE *Stream) noexcept
        : ErrorStream(Stream) {}

        static auto null() noexcept {
            return DiagnosticsEngine(NULL);
        }

        __printflike(2, 3)
        void emitError(const char *Message, ...) noexcept;

        __printflike(2, 3)
        void emitWarning(const char *Message, ...) noexcept;
    };
}