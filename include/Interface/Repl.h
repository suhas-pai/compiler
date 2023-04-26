/*
 * Interface/REPL.h
 */

#pragma once
#include <string>

namespace Interface {
    void
    SetupRepl(std::string_view Prompt,
              const std::function<bool(std::string_view)> &Callback) noexcept;
}