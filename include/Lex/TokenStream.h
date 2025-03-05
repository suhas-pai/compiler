/*
 * Lex/TokenStream.h
 * © suhas pai
 */

#pragma once
#include <span>

#include "Lex/Token.h"
#include "Lex/TokenBuffer.h"

namespace Lex {
    struct TokenStream {
    protected:
        const TokenBuffer &TokenBuffer;
        uint32_t Index = 0;
    public:
        explicit TokenStream(const struct TokenBuffer &TokenBuffer) noexcept
        : TokenBuffer(TokenBuffer) {}

        [[nodiscard]] constexpr auto position() const noexcept {
            return this->Index;
        }

        [[nodiscard]] constexpr auto reachedEof() const noexcept {
            return this->Index == TokenBuffer.getTokenList().size();
        }

        [[nodiscard]]
        constexpr auto tokenContent(const Lex::Token Token) const noexcept {
            return Token.getString(this->TokenBuffer.getSourceBuffer().text());
        }

        [[nodiscard]]
        constexpr auto tokenKeyword(const Lex::Token Token) const noexcept {
            assert(Token.Kind == Lex::TokenKind::Keyword);
            return Lex::KeywordTokenGetKeyword(this->tokenContent(Token));
        }

        [[nodiscard]] constexpr auto getCurrOrPrevLoc() const noexcept {
            if (!this->reachedEof()) {
                return this->TokenBuffer.getTokenList()[this->position()].Loc;
            }

            return this->TokenBuffer.getTokenList()[this->position() - 1].Loc;
        }

        constexpr auto goBack(const uint32_t Count = 1) noexcept -> bool {
            if (this->position() >= Count) {
                this->Index -= Count;
                return true;
            }

            return false;
        }

        [[nodiscard]] constexpr auto peek() const noexcept
            -> std::optional<Lex::Token>
        {
            const auto TokenList = this->TokenBuffer.getTokenList();
            if (!this->reachedEof()) {
                return TokenList[this->position()];
            }

            return std::nullopt;
        }

        [[nodiscard]]
        constexpr auto peekIs(const Lex::TokenKind Kind) const noexcept {
            if (const auto TokenOpt = this->peek()) {
                return TokenOpt->Kind == Kind;
            }

            return false;
        }

        [[nodiscard]] constexpr auto
        peekIsOneOf(const std::span<const Lex::TokenKind> Kinds) const noexcept
        {
            if (const auto TokenOpt = this->peek()) {
                for (const auto Kind : Kinds) {
                    if (TokenOpt->Kind == Kind) {
                        return true;
                    }
                }
            }

            return false;
        }

        constexpr auto consume() noexcept -> std::optional<Lex::Token> {
            if (!this->reachedEof()) {
                this->Index++;
                return this->TokenBuffer.getTokenList()[this->position() - 1];
            }

            return std::nullopt;
        }

        [[nodiscard]]
        constexpr auto current() const noexcept -> std::optional<Lex::Token> {
            if (!this->reachedEof()) {
                return this->TokenBuffer.getTokenList()[this->position()];
            }

            return std::nullopt;
        }

        [[nodiscard]]
        constexpr auto consumeIfIs(const Lex::TokenKind Kind) noexcept
            -> std::optional<Lex::Token>
        {
            const auto TokenOpt = this->peek();
            if (!TokenOpt.has_value()) {
                return std::nullopt;
            }

            const auto Token = TokenOpt.value();
            if (Token.Kind != Kind) {
                return std::nullopt;
            }

            return this->consume();
        }

        [[nodiscard]] constexpr auto
        consumeIfOneOf(const std::span<const Lex::TokenKind> Kinds) noexcept
            -> std::optional<Lex::Token>
        {
            const auto TokenOpt = this->peek();
            if (!TokenOpt.has_value()) {
                return std::nullopt;
            }

            const auto Token = TokenOpt.value();
            for (const auto Kind : Kinds) {
                if (Token.Kind == Kind) {
                    return this->consume();
                }
            }

            return std::nullopt;
        }

        constexpr auto proceedToAndConsume(const Lex::TokenKind Kind) noexcept
            -> std::optional<Lex::Token>
        {
            while (const auto TokenOpt = this->consume()) {
                if (TokenOpt->Kind == Kind) {
                    return TokenOpt;
                }
            }

            return std::nullopt;
        }

        constexpr auto
        proceedToAndConsumeOneOf(
            const std::span<const Lex::TokenKind> KindList) noexcept
                -> std::optional<Lex::Token>
        {
            while (const auto TokenOpt = this->consume()) {
                for (const auto Kind : KindList) {
                    if (TokenOpt->Kind == Kind) {
                        return TokenOpt;
                    }
                }
            }

            return std::nullopt;
        }
    };
}
