/*
 * Lexer/Tokenizer.h
 */

#pragma once
#include <vector>

#include "Diag/Consumer.h"
#include "Lex/Infos.h"

#include "Token.h"

namespace Lex {
    struct Tokenizer {
    public:
        enum class State {
            Start,

            CharLiteral,
            StringLiteral,
            IntegerLiteral,
            IntegerLiteralWithSuffix,
            FloatLiteral,
            Identifier,
            DotIdentifier,

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
            ShiftLeft,
            ShiftRight,
            Equal,
            Exclamation,

            Dot,
            DotDot,
        };
    protected:
        std::string_view Text;

        SourceLocation Loc;
        DiagnosticConsumer &Diag;

        LineInfo CurrentLineInfo;

        [[nodiscard]] constexpr auto peek() const noexcept -> char {
            if (this->Loc.Index >= this->Text.length()) {
                return '\0';
            }

            return this->Text.at(this->Loc.Index);
        }

        constexpr auto consume(const uint32_t Skip = 0) noexcept -> char {
            uint32_t End = 0;
            if (__builtin_add_overflow(this->Loc.Index, Skip, &End)) {
                return '\0';
            }

            if (End >= this->Text.length()) {
                return '\0';
            }

            this->Loc.Index += Skip + 1;
            return this->Text.at(this->Loc.Index - 1);
        }
    public:
        constexpr explicit
        Tokenizer(const std::string_view Text,
                  DiagnosticConsumer &DiagConsumer) noexcept
        : Text(Text), Diag(DiagConsumer), CurrentLineInfo() {}

        [[nodiscard]] constexpr auto getIndex() const noexcept {
            return this->Loc.Index;
        }

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return this->Loc;
        }

        [[nodiscard]] auto next() noexcept
            -> std::pair<Lex::Token, Lex::LineInfo>;

        [[nodiscard]] auto createList() noexcept
            -> std::optional<std::vector<Lex::Token>>
        {
            auto Result = std::vector<Lex::Token>();
            while (true) {
                const auto [Token, LineInfo] = this->next();
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