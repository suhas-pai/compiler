/*
 * Lex/Tokenizer.cpp
 */

#include "Lex/Tokenizer.h"
#include "Parse/String.h"

namespace Lex {
    auto Tokenizer::next() noexcept -> Token {
        auto State = State::Start;
        auto Result = Token();

        for (auto Char = this->consume();;
             Char = this->consume(), Result.End.Value++)
        {
            if (Char == '\0') {
                if (State == State::Identifier) {
                    const auto KeywordOpt =
                        KeywordToLexemeMap.keyFor(Result.getString(Text));

                    if (KeywordOpt.has_value()) {
                        Result.Kind = TokenKind::Keyword;
                    }

                    goto done;
                }

                if (State != State::Start) {
                    break;
                }

                if (Index == Text.length()) {
                    Result = Token::eof();
                    break;
                }

                Diag.emitError("Tokenizer::next(): Unexpected null char\n");
                return Token::invalid();
            }

            switch (State) {
                case State::Start:
                    Result.Loc.Value = Index - 1;
                    switch (Char) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\r':
                            continue;
                        case '\'':
                            State = State::CharLiteral;
                            break;
                        case '"':
                            State = State::StringLiteral;
                            break;
                        case 'a'...'z':
                        case 'A'...'Z':
                        case '_':
                            State = State::Identifier;
                            Result.Kind = TokenKind::Identifier;

                            continue;
                        case '0'...'9':
                            State = State::NumberLiteral;
                            Result.Kind = TokenKind::NumberLiteral;

                            continue;
                        case '+':
                            State = State::Plus;
                            Result.Kind = TokenKind::Plus;

                            continue;
                        case '-':
                            State = State::Minus;
                            Result.Kind = TokenKind::Minus;

                            continue;
                        case '*':
                            State = State::Star;
                            Result.Kind = TokenKind::Star;

                            continue;
                        case '%':
                            State = State::Percent;
                            Result.Kind = TokenKind::Percent;

                            continue;
                        case '/':
                            State = State::Slash;
                            Result.Kind = TokenKind::Slash;

                            continue;
                        case '^':
                            State = State::Caret;
                            Result.Kind = TokenKind::Caret;

                            break;
                        case '&':
                            State = State::Ampersand;
                            Result.Kind = TokenKind::Ampersand;
                            break;
                        case '|':
                            State = State::Pipe;
                            Result.Kind = TokenKind::Pipe;

                            break;
                        case '~':
                            State = State::Tilde;
                            Result.Kind = TokenKind::Tilde;

                            break;
                        case '<':
                            State = State::LessThan;
                            Result.Kind = TokenKind::LessThan;

                            break;
                        case '>':
                            State = State::GreaterThan;
                            Result.Kind = TokenKind::GreaterThan;

                            break;
                        case '=':
                            State = State::Equal;
                            Result.Kind = TokenKind::Equal;

                            break;
                        case '!':
                            State = State::Exclamation;
                            Result.Kind = TokenKind::Exclamation;

                            break;
                        case '(':
                            Result.Kind = TokenKind::OpenParen;
                            goto done;
                        case ')':
                            Result.Kind = TokenKind::CloseParen;
                            goto done;
                        case '{':
                            Result.Kind = TokenKind::LeftCurlyBrace;
                            goto done;
                        case '}':
                            Result.Kind = TokenKind::RightCurlyBrace;
                            goto done;
                        case '[':
                            Result.Kind = TokenKind::LeftSquareBracket;
                            goto done;
                        case ']':
                            Result.Kind = TokenKind::RightSquareBracket;
                            goto done;
                        case ',':
                            Result.Kind = TokenKind::Comma;
                            goto done;
                        case '.':
                            Result.Kind = TokenKind::Dot;
                            goto done;
                        case ':':
                            Result.Kind = TokenKind::Colon;
                            goto done;
                        case ';':
                            Result.Kind = TokenKind::Semicolon;
                            goto done;
                        default:
                            Diag.emitError("Unrecognized token: %c\n", Char);
                            Result.Kind = TokenKind::Invalid;

                            goto done;
                    }

                    break;
                case State::CharLiteral:
                    switch (Char) {
                        case '\\': {
                            Index++;
                            break;
                        }
                        case '\'':
                            goto done;
                        default:
                            break;
                    }

                    break;
                case State::StringLiteral:
                    switch (Char) {
                        case '\\':
                            Index++;
                            break;
                        case '"':
                            goto done;
                        default:
                            break;
                    }

                    break;
                case State::NumberLiteral:
                    switch (Char) {
                        case '0'...'9':
                        case 'a'...'f':
                        case 'A'...'F':
                        case 'o':
                        case 'O':
                        case 'x':
                        case 'X':
                            break;
                        case '.':
                            Diag.emitError("Floating-point not yet supported");
                            Result.Kind = TokenKind::Invalid;

                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
                case State::Identifier:
                    switch (Char) {
                        case '0'...'9':
                        case 'a'...'z':
                        case 'A'...'Z':
                        case '_':
                            break;
                        default:
                            Index--;
                            Result.End.Value = Index;

                            const auto KeywordOpt =
                                KeywordToLexemeMap.keyFor(
                                    Result.getString(Text));

                            if (KeywordOpt.has_value()) {
                                Result.Kind = TokenKind::Keyword;
                            }

                            goto done;
                    }

                    break;
                case State::Plus:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::PlusEqual;
                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
                case State::Minus:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::MinusEqual;
                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
                case State::Star:
                    switch (Char) {
                        case '*':
                            Result.Kind = TokenKind::DoubleStar;
                            goto done;
                        case '=':
                            Result.Kind = TokenKind::StarEqual;
                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
                case State::Percent:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::PercentEqual;
                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
                case State::Slash:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::SlashEqual;
                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
                case State::Caret:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::CaretEqual;
                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
                case State::Ampersand:
                    switch (Char) {
                        case '&':
                            Result.Kind = TokenKind::DoubleAmpersand;
                            goto done;
                        case '=':
                            Result.Kind = TokenKind::AmpersandEqual;
                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
                case State::Pipe:
                    switch (Char) {
                        case '|':
                            Result.Kind = TokenKind::DoublePipe;
                            goto done;
                        case '=':
                            Result.Kind = TokenKind::PipeEqual;
                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
                case State::Tilde:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::TildeEqual;
                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
                case State::LessThan:
                    switch (Char) {
                        case '<':
                            State = State::Shl;
                            Result.Kind = TokenKind::Shl;

                            continue;
                        case '=':
                            Result.Kind = TokenKind::LessThanOrEqual;
                            goto done;
                        default:
                            Index--;
                            State = State::Start;

                            continue;
                    }

                    break;
                case State::GreaterThan:
                    switch (Char) {
                        case '>':
                            State = State::Shr;
                            Result.Kind = TokenKind::Shr;

                            continue;
                        case '=':
                            Result.Kind = TokenKind::GreaterThanOrEqual;
                            goto done;
                        default:
                            Index--;
                            State = State::Start;

                            continue;
                    }

                    break;
                case State::Shl:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::ShlEqual;
                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
                case State::Shr:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::ShrEqual;
                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
                case State::Equal:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::DoubleEqual;
                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
                case State::Exclamation:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::NotEqual;
                            goto done;
                        default:
                            Index--;
                            goto done;
                    }

                    break;
            }
        }

    done:
        State = State::Start;
        Result.End.Value = Index;

        return Result;
    }
}