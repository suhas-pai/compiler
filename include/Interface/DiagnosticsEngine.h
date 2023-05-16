/*
 * Interface/DiagnosticsEngine.h
 */

#pragma once
#include <stdio.h>

#include <sys/cdefs.h>
#include "Basic/Macros.h"

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

        __printflike(2, 3)
        void reportError(const char *Message, ...) noexcept;

        __printflike(2, 3)
        void emitError(const char *Message, ...) noexcept;

        __printflike(2, 3)
        void emitWarning(const char *Message, ...) noexcept;
    };
}