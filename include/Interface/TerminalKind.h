/*
 * Interface/TerminalKind.h
 */

#pragma once

namespace Interface {
    enum class TerminalKind {
        Normal,
        Repl
    };

    auto GetTerminalKind() noexcept -> TerminalKind;
    auto SetTerminalKind(TerminalKind TermKind) noexcept -> void;
}