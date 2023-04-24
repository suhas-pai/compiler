/*
 * Lex/Tokenizer.cpp
 */

#include <cassert>

#include "Lex/Tokenizer.h"
#include "Lex/Keyword.h"
#include "Lex/Token.h"
#include "Parse/String.h"

namespace Lex {
    auto Tokenizer::next() noexcept -> Token {
        auto State = State::Start;
        auto Result = Token();

        for (auto Char = this->consume();; Char = this->consume()) {
            if (Char == '\0') {
                if (State != State::Start) {
                    break;
                }

                if (Index == Text.length()) {
                    Result = Token::eof();
                    break;
                }

                printf("Tokenizer::next(): Unexpected end of file\n");
                exit(1);
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
                            continue;
                        case '0'...'9':
                            State = State::NumberLiteral;
                            continue;
                        case '+':
                            State = State::Plus;
                            continue;
                        case '-':
                            State = State::Minus;
                            continue;
                        case '*':
                            State = State::Star;
                            continue;
                        case '%':
                            State = State::Percent;
                            continue;
                        case '/':
                            State = State::Slash;
                            continue;
                        case '^':
                            State = State::Caret;
                            break;
                        case '&':
                            State = State::Ampersand;
                            break;
                        case '|':
                            State = State::Pipe;
                            break;
                        case '~':
                            State = State::Tilde;
                            break;
                        case '<':
                            State = State::LessThan;
                            break;
                        case '>':
                            State = State::GreaterThan;
                            break;
                        case '=':
                            State = State::Equal;
                            break;
                        case '!':
                            State = State::Exclamation;
                            break;
                        case '(':
                            Result.Kind = TokenKind::LeftParen;
                            goto done;
                        case ')':
                            Result.Kind = TokenKind::RightParen;
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
                            printf("Unrecognized token: %c\n", Char);
                            exit(1);
                    }

                    break;
                case State::CharLiteral:
                    switch (Char) {
                        case '\\': {
                            Index++;
                            const auto Peek = this->peek();

                            if (!Peek || !Parse::VerifyEscapeSequence(Peek)) {
                                printf("Tokenizer: Unexpected end of file\n");
                                exit(1);
                            }

                            break;
                        }
                        case '\'':
                            if (State == State::CharLiteral) {
                                Result.Kind = TokenKind::CharLiteral;
                                goto done;
                            }

                            break;
                        default:
                            break;
                    }
                case State::StringLiteral:
                    switch (Char) {
                        case '\\': {
                            Index++;
                            const auto Peek = this->peek();

                            if (!Peek || !Parse::VerifyEscapeSequence(Peek)) {
                                printf("Tokenizer: Unexpected end of file\n");
                                exit(1);
                            }

                            break;
                        }
                        case '"':
                            if (State == State::StringLiteral) {
                                Result.Kind = TokenKind::StringLiteral;
                                goto done;
                            }

                            break;
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
                            assert(false && "Floating-point not yet supported");
                        default:
                            Index--;
                            Result.Kind = TokenKind::IntegerLiteral;

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
                            } else {
                                Result.Kind = TokenKind::Identifier;
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
                            Result.Kind = TokenKind::Plus;

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
                            Result.Kind = TokenKind::Minus;

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
                            Result.Kind = TokenKind::Star;

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
                            Result.Kind = TokenKind::Percent;

                            goto done;
                    }

                    break;
                case State::Slash:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::SlashEqual;
                            break;
                        default:
                            Index--;
                            Result.Kind = TokenKind::Slash;

                            goto done;
                    }

                    break;
                case State::Caret:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::CaretEqual;
                            break;
                        default:
                            Index--;
                            Result.Kind = TokenKind::Caret;

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
                            Result.Kind = TokenKind::Ampersand;

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
                            Result.Kind = TokenKind::Pipe;

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
                            Result.Kind = TokenKind::Tilde;

                            goto done;
                    }

                    break;
                case State::LessThan:
                    switch (Char) {
                        case '<':
                            State = State::Shl;
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
                            Result.Kind = TokenKind::Shl;

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
                            Result.Kind = TokenKind::Shr;

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
                            Result.Kind = TokenKind::Equal;

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
                            Result.Kind = TokenKind::Exclamation;

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