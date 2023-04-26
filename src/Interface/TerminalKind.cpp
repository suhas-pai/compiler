/*
 * Interface/TerminalKind.cpp
 */

#include "Interface/TerminalKind.h"

namespace Interface {
    TerminalKind gTermKind = TerminalKind::Normal;

    auto GetTerminalKind() noexcept -> TerminalKind {
        return gTermKind;
    }

    auto SetTerminalKind(TerminalKind TermKind) noexcept -> void {
        gTermKind = TermKind;
    }
}