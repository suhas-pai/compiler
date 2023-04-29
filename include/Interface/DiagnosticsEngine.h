/*
 * Interface/DiagnosticsEngine.h
 */

#pragma once
#include <sys/cdefs.h>

namespace Interface {
    struct DiagnosticsEngine {
    public:
        constexpr explicit DiagnosticsEngine() noexcept = default;

        __printflike(2, 3)
        void emitError(const char *Message, ...) noexcept;

        __printflike(2, 3)
        void emitWarning(const char *Message, ...) noexcept;
    };
}