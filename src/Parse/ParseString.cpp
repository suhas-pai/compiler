/*
 * Parse/ParseString.cpp
 * © suhas pai
 */

#include "Basic/Lexer.h"
#include "Misc/Utils.h"

#include "Parse/ParseString.h"

namespace Parse {
    static auto
    VerifyEscapeSequence(const char Ch, Lexer &Lexer) noexcept -> char {
        switch (Ch) {
            case 'a':
                return '\a';
            case 'b':
                return '\b';
            case 'f':
                return '\f';
            case 'n':
                return '\n';
            case 'r':
                return '\r';
            case 't':
                return '\t';
            case 'v':
                return '\v';
            case 'x': {
                const auto First = Lexer.consume();
                const auto Second = Lexer.consume();

                if (First == '\0' || Second == '\0') {
                    return '\0';
                }

                // TODO: Move logic to separate function.
                const auto FirstDigit = Misc::hexdigit_to_number(First);
                const auto SecondDigit = Misc::hexdigit_to_number(Second);

                return static_cast<char>((FirstDigit << 4) | SecondDigit);
            }
            case '\\':
                return '\\';
            case '\'':
                return '\'';
            case '"':
                return '"';
        }

        return '\0';
    }

    auto
    ParseCharLiteral(ParseContext &Context, const Lex::Token Token) noexcept
        -> AST::CharLiteral *
    {
        auto &Diag = Context.Diag;
        auto &TokenStream = Context.TokenStream;

        const auto TokenString = TokenStream.tokenContent(Token);
        auto Lexer = ::Lexer(TokenString.substr(1));

        const auto FirstChar = Lexer.consume();
        if (FirstChar == '\'') {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = Token.Loc,
                .Message = "Character Literals cannot be empty. Use \'\\0\' to "
                           "denote a null character"
            });

            return nullptr;
        }


        if (FirstChar != '\\') {
            if (Lexer.consume() != '\'') {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = Token.Loc,
                    .Message = "Use double quotes to store strings with "
                               "multiple characters"
                });

                return nullptr;
            }

            return new AST::CharLiteral(Token.Loc, FirstChar);
        }

        if (FirstChar == '\\' && TokenString.size() == 3) {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = Token.Loc,
                .Message = "Expected another character to parse "
                           "escape-sequence. Use '\\\\' to store a backs-slash"
            });

            return nullptr;
        }

        const auto EscapedChar = VerifyEscapeSequence(Lexer.consume(), Lexer);
        if (EscapedChar == '\0') {
            Diag.consume({
                .Level = DiagnosticLevel::Error,
                .Location = Token.Loc,
                .Message = "Invalid escape-sequence"
            });

            return nullptr;
        }

        return new AST::CharLiteral(Token.Loc, EscapedChar);
    }

    auto
    ParseStringLiteral(ParseContext &Context,
                       const Lex::Token StringToken) noexcept
        -> AST::StringLiteral *
    {
        auto &TokenStream = Context.TokenStream;
        auto &Diag = Context.Diag;

        const auto StringTokenContent = TokenStream.tokenContent(StringToken);
        const auto StringTokenString =
            StringTokenContent.substr(1, StringTokenContent.length() - 2);

        auto String = std::string();
        auto Lexer = ::Lexer(StringTokenString);

        while (!Lexer.finished()) {
            const auto Char = Lexer.consume();
            if (Char != '\\') {
                String.push_back(Char);
                continue;
            }

            const auto NextChar = Lexer.consume();
            if (NextChar == '\0') {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = StringToken.Loc,
                    .Message =
                        "Unexpected end of string after escape character. "
                        "To use a backslash, use '\\\\' as the escape sequence"
                });

                return nullptr;
            }

            const auto EscapedChar = VerifyEscapeSequence(NextChar, Lexer);
            if (EscapedChar == '\0') {
                Diag.consume({
                    .Level = DiagnosticLevel::Error,
                    .Location = StringToken.Loc,
                    .Message = "Invalid escape sequence"
                });

                return nullptr;
            }

            String.push_back(EscapedChar);
        }

        return new AST::StringLiteral(StringToken.Loc, std::move(String));
    }
}