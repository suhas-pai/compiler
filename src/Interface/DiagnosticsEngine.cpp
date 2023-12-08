/*
 * Interface/DiagnosticsEngine.cpp
 */

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#include "Interface/ANSI.h"
#include "Interface/DiagnosticsEngine.h"
#include "Interface/TerminalKind.h"

namespace Interface {
    void DiagnosticsEngine::emitError(const char *const Message, ...) noexcept {
        if (ErrorStream == nullptr) {
            return;
        }

        va_list List;
        fputs(BRED "Error: " CRESET, stderr);

        va_start(List, Message);
        vfprintf(stderr, Message, List);
        va_end(List);

        fputc('\n', stderr);
        if (GetTerminalKind() != TerminalKind::Repl) {
            exit(0);
        }
    }

    void
    DiagnosticsEngine::emitInternalError(const char *const Message,
                                         ...) noexcept
    {
        if (ErrorStream == nullptr) {
            return;
        }

        va_list List;
        fputs(BRED "Internal Error: " CRESET, stderr);

        va_start(List, Message);
        vfprintf(stderr, Message, List);
        va_end(List);

        fputc('\n', stderr);
        if (GetTerminalKind() != TerminalKind::Repl) {
            exit(0);
        }
    }

    void
    DiagnosticsEngine::emitWarning(const char *const Message, ...) noexcept {
        if (ErrorStream == nullptr) {
            return;
        }

        va_list List;
        fputs(BYEL "Warning: " CRESET, stderr);

        va_start(List, Message);
        vfprintf(stderr, Message, List);
        va_end(List);

        fputc('\n', stderr);
    }
}