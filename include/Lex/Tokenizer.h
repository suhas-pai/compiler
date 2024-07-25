/*
 * Lexer/Tokenizer.h
 */

#pragma once

#include <vector>

#include "Interface/DiagnosticsEngine.h"
#include "Token.h"

namespace Lex {
    struct Tokenizer {
    public:
        enum class State {
            Start,

            CharLiteral,
            StringLiteral,
            IntegerLiteral,
            FloatLiteral,
            Identifier,

            Plus,
            Minus,
            Star,
            Percent,
            Slash,
            Caret,
            Ampersand,
            Pipe,
            Tilde,
            LessThan,
            GreaterThan,
            Shl,
            Shr,
            Equal,
            Exclamation,
        };
    protected:
        std::string_view Text;
        uint64_t Index = 0;
        Interface::DiagnosticsEngine &Diag;

        [[nodiscard]] constexpr auto peek() const noexcept -> char {
            if (Index >= Text.length()) {
                return '\0';
            }

            return Text.at(Index);
        }

        constexpr auto consume(const uint64_t Skip = 0) noexcept -> char {
            if (Index + Skip >= Text.length()) {
                return '\0';
            }

            Index += Skip + 1;
            return Text.at(Index - 1);
        }
    public:
        constexpr explicit
        Tokenizer(const std::string_view Text,
                  Interface::DiagnosticsEngine &Diag) noexcept
        : Text(Text), Diag(Diag) {}

        [[nodiscard]] constexpr auto index() const noexcept {
            return Index;
        }

        [[nodiscard]] auto next() noexcept -> Lex::Token;
        [[nodiscard]] auto createList() noexcept
            -> std::optional<std::vector<Lex::Token>>
        {
            auto Result = std::vector<Lex::Token>();
            while (true) {
                const auto Token = this->next();
                if (Token.Kind == TokenKind::EOFToken) {
                    return Result;
                }

                if (Token.Kind == TokenKind::Invalid) {
                    return std::nullopt;
                }

                Result.push_back(Token);
            }

            return std::nullopt;
        }
    };
}