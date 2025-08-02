/*
 * Lex/TokenStream.cpp
 * © suhas pai
 */

#include "Lex/TokenStream.h"

namespace Lex {
    auto TokenStream::findNextAndConsume(const Lex::TokenKind Kind) noexcept
        -> std::expected<Lex::Token, FindError>
    {
        auto Stack = std::vector<Lex::Token>();
        while (const auto TokenOpt = this->consume()) {
            const auto Token = TokenOpt.value();
            if (Token.Kind == Kind) {
                if (Stack.empty()) {
                    return Token;
                }
            }

            if (Token.Kind == Lex::TokenKind::OpenCurlyBrace ||
                Token.Kind == Lex::TokenKind::OpenParen ||
                Token.Kind == Lex::TokenKind::LeftSquareBracket)
            {
                Stack.emplace_back(Token);
                continue;
            }

            if (Token.Kind == Lex::TokenKind::CloseCurlyBrace ||
                Token.Kind == Lex::TokenKind::CloseParen ||
                Token.Kind == Lex::TokenKind::RightSquareBracket)
            {
                if (Stack.empty()) {
                    return std::unexpected(FindError::UnexpectedClosingToken);
                }

                const auto Top = Stack.back();
                Stack.pop_back();

                if ((Top.Kind == Lex::TokenKind::OpenCurlyBrace &&
                        Token.Kind != Lex::TokenKind::CloseCurlyBrace) ||
                    (Top.Kind == Lex::TokenKind::OpenParen &&
                        Token.Kind != Lex::TokenKind::CloseParen) ||
                    (Top.Kind == Lex::TokenKind::LeftSquareBracket &&
                        Token.Kind != Lex::TokenKind::RightSquareBracket))
                {
                    return std::unexpected(FindError::MismatchClosingToken);
                }
            }
        }

        if (Stack.empty()) {
            return std::unexpected(FindError::NotFound);
        }

        return std::unexpected(FindError::UnclosedToken);
    }

    auto
    TokenStream::findNextAndConsumeOneOf(
        const std::initializer_list<Lex::TokenKind> KindList) noexcept
            -> std::expected<Lex::Token, FindError>
    {
        auto Stack = std::vector<Lex::Token>();
        while (const auto TokenOpt = this->consume()) {
            const auto Token = TokenOpt.value();
            if (Stack.empty()) {
                for (const auto Kind : KindList) {
                    if (Token.Kind == Kind) {
                        return Token;
                    }
                }
            }

            if (Token.Kind == Lex::TokenKind::OpenCurlyBrace ||
                Token.Kind == Lex::TokenKind::OpenParen ||
                Token.Kind == Lex::TokenKind::LeftSquareBracket)
            {
                Stack.emplace_back(Token);
                continue;
            }

            if (Token.Kind == Lex::TokenKind::CloseCurlyBrace ||
                Token.Kind == Lex::TokenKind::CloseParen ||
                Token.Kind == Lex::TokenKind::RightSquareBracket)
            {
                if (Stack.empty()) {
                    return std::unexpected(FindError::UnclosedToken);
                }

                const auto Top = Stack.back();
                Stack.pop_back();

                if ((Top.Kind == Lex::TokenKind::OpenCurlyBrace &&
                        Token.Kind != Lex::TokenKind::CloseCurlyBrace) ||
                    (Top.Kind == Lex::TokenKind::OpenParen &&
                        Token.Kind != Lex::TokenKind::CloseParen) ||
                    (Top.Kind == Lex::TokenKind::LeftSquareBracket &&
                        Token.Kind != Lex::TokenKind::RightSquareBracket))
                {
                    return std::unexpected(FindError::MismatchClosingToken);
                }
            }
        }

        if (Stack.empty()) {
            return std::unexpected(FindError::NotFound);
        }

        return std::unexpected(FindError::UnclosedToken);
    }

    auto
    TokenStream::findNextNotOneOfAndGoToToken(
        const std::initializer_list<Lex::TokenKind> KindList) noexcept
            -> std::expected<Lex::Token, FindError>
    {
        auto Stack = std::vector<Lex::Token>();
        while (const auto TokenOpt = this->peek()) {
            const auto Token = TokenOpt.value();
            if (Stack.empty()) {
                bool found = false;
                for (const auto Kind : KindList) {
                    if (Token.Kind == Kind) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    return Token;
                }
            }

            this->consume();
            if (Token.Kind == Lex::TokenKind::OpenCurlyBrace ||
                Token.Kind == Lex::TokenKind::OpenParen ||
                Token.Kind == Lex::TokenKind::LeftSquareBracket)
            {
                Stack.emplace_back(Token);
                continue;
            }

            if (Token.Kind == Lex::TokenKind::CloseCurlyBrace ||
                Token.Kind == Lex::TokenKind::CloseParen ||
                Token.Kind == Lex::TokenKind::RightSquareBracket)
            {
                if (Stack.empty()) {
                    return std::unexpected(FindError::UnclosedToken);
                }

                const auto Top = Stack.back();
                Stack.pop_back();

                if ((Top.Kind == Lex::TokenKind::OpenCurlyBrace &&
                        Token.Kind != Lex::TokenKind::CloseCurlyBrace) ||
                    (Top.Kind == Lex::TokenKind::OpenParen &&
                        Token.Kind != Lex::TokenKind::CloseParen) ||
                    (Top.Kind == Lex::TokenKind::LeftSquareBracket &&
                        Token.Kind != Lex::TokenKind::RightSquareBracket))
                {
                    return std::unexpected(FindError::MismatchClosingToken);
                }
            }
        }

        if (Stack.empty()) {
            return std::unexpected(FindError::NotFound);
        }

        return std::unexpected(FindError::UnclosedToken);
    }
}
