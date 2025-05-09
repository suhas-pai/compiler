/*
 * Interface/Repl.cpp
 */

#include <format>
#include <print>

#include <readline/history.h>
#include <readline/readline.h>

#include "Basic/ANSI.h"
#include "Misc/Repl.h"

namespace Interface {
    void
    SetupRepl(const std::string_view Prompt,
              const std::function<bool(std::string_view)> &Callback) noexcept
    {
        /*
        rl_bind_key('\t', rl_complete);
        rl_bind_key('\n', rl_insert);
        rl_bind_key('\r', rl_insert);
        rl_bind_key('`', rl_complete);
        rl_bind_key('~', rl_complete);
        rl_bind_key('?', rl_complete);
        rl_bind_key('*', rl_complete);
        rl_bind_key('/', rl_complete);
        rl_bind_key(' ', rl_complete);
        rl_bind_key('-', rl_complete);
        rl_bind_key('+', rl_complete);
        rl_bind_key('=', rl_complete);
        rl_bind_key('[', rl_complete);
        rl_bind_key(']', rl_complete);
        rl_bind_key('{', rl_complete);
        rl_bind_key('}', rl_complete);
        rl_bind_key('\\', rl_complete);
        rl_bind_key('|', rl_complete);
        rl_bind_key(';', rl_complete);
        rl_bind_key(':', rl_complete);
        rl_bind_key(',', rl_complete);

        for (char C = 'a'; C <= 'z'; ++C) {
            rl_bind_key(C, rl_complete);
            rl_bind_key(C + 'A' - 'a', rl_complete);
        }
        */

        rl_set_keymap(rl_make_bare_keymap());
        using_history();

        const auto FullPrompt =
            std::format(ANSI_BHBLU "{}> " ANSI_CRESET, Prompt);

        while (true) {
            const auto InputCStr = readline(FullPrompt.c_str());
            if (InputCStr == nullptr) {
                std::print("\n");
                continue;
            }

            if (*InputCStr == '\0') {
                continue;
            }

            const auto Input = std::string_view(InputCStr);
            add_history(Input.data());

            if (Input == "exit" || Input == "quit") {
                exit(0);
            } else if (Input == "clear") {
                system("clear");
                continue;
            }

            if (!Callback(Input)) {
                break;
            }
        }
    }
}