/*
 * Interface/Repl.cpp
 */

#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>

#include "Interface/ANSI.h"
#include "Interface/Repl.h"
#include "Interface/TerminalKind.h"

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

        SetTerminalKind(TerminalKind::Repl);
        rl_set_keymap(rl_make_bare_keymap());

        using_history();

        auto FullPrompt = std::string(Prompt) + "> " + CRESET;
        FullPrompt.insert(0, BHBLK);

        while (true) {
            const auto InputCStr = readline(FullPrompt.c_str());
            if (InputCStr == NULL) {
                fputc('\n', stdout);
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