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
    void
    DiagnosticsEngine::emitError(const SourceLocation Loc,
                                 const char *const Message,
                                 ...) noexcept
    {
        (void)Loc;
        if (ErrorStream == nullptr) {
            return;
        }

        va_list List;
        fputs(BRED "Error: " CRESET, ErrorStream);

        va_start(List, Message);
        vfprintf(ErrorStream, Message, List);
        va_end(List);

        fputc('\n', ErrorStream);
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
        fputs(BRED "Internal Error: " CRESET, ErrorStream);

        va_start(List, Message);
        vfprintf(ErrorStream, Message, List);
        va_end(List);

        fputc('\n', ErrorStream);
        if (GetTerminalKind() != TerminalKind::Repl) {
            exit(0);
        }
    }

    void
    DiagnosticsEngine::emitWarning(const SourceLocation Loc,
                                   const char *const Message, ...) noexcept
    {
        (void)Loc;
        if (ErrorStream == nullptr) {
            return;
        }

        va_list List;
        fputs(BYEL "Warning: " CRESET, ErrorStream);

        va_start(List, Message);
        vfprintf(ErrorStream, Message, List);
        va_end(List);

        fputc('\n', ErrorStream);
    }
}