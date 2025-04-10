/*
 * Misc/REPL.h
 */

#pragma once

#include <functional>
#include <string_view>

namespace Interface {
    void
    SetupRepl(std::string_view Prompt,
              const std::function<bool(std::string_view)> &Callback) noexcept;
}