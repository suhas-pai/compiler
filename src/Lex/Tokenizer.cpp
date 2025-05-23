/*
 * Lex/Tokenizer.cpp
 */

#include "Lex/Tokenizer.h"

namespace Lex {
    auto Tokenizer::next() noexcept -> std::pair<Lex::Token, Lex::LineInfo> {
        auto State = State::Start;
        auto Result = Token();

        for (auto Char = this->consume();; Char = this->consume()) {
            if (Char == '\0') {
                if (State == State::Identifier) {
                    const auto TokenKindOpt =
                        KeywordToLexemeMap.keyFor(Result.getString(Text));

                    if (TokenKindOpt.has_value()) {
                        Result.Kind = TokenKind::Keyword;
                    }

                    goto done;
                }

                if (State != State::Start) {
                    break;
                }

                if (this->Loc.Index == this->Text.length()) {
                    Result = Token::eof();
                    break;
                }

                return std::pair(Token::invalid(), this->CurrentLineInfo);
            }

            switch (State) {
                case State::Start:
                    Result.Loc.Index = this->Loc.Index - 1;
                    switch (Char) {
                        case '\n':
                            this->Loc.Row++;
                            this->Loc.Column = 0;
                            this->CurrentLineInfo = {
                                .ByteOffset = this->Loc.Index
                            };

                            if (this->Loc.Row > SourceLocation::RowLimit) {
                                Diag.consume({
                                    .Level = DiagnosticLevel::Error,
                                    .Location = this->Loc,
                                    .Message = "Line is too long"
                                });

                                return std::pair(Token::invalid(),
                                                 this->CurrentLineInfo);
                            }

                            continue;
                        case ' ':
                        case '\t':
                        case '\r':
                            break;
                        case '\'':
                            State = State::CharLiteral;

                            Result.Kind = TokenKind::CharLiteral;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '"':
                            State = State::StringLiteral;

                            Result.Kind = TokenKind::StringLiteral;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case 'a'...'z':
                        case 'A'...'Z':
                        case '_':
                            State = State::Identifier;
                            Result.Kind = TokenKind::Identifier;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '0'...'9':
                            State = State::IntegerLiteral;

                            Result.Kind = TokenKind::IntegerLiteral;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '+':
                            State = State::Plus;

                            Result.Kind = TokenKind::Plus;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '-':
                            State = State::Minus;

                            Result.Kind = TokenKind::Minus;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '*':
                            State = State::Star;

                            Result.Kind = TokenKind::Star;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '%':
                            State = State::Percent;

                            Result.Kind = TokenKind::Percent;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '/':
                            State = State::Slash;

                            Result.Kind = TokenKind::Slash;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '^':
                            State = State::Caret;
                            this->Loc.Column++;

                            Result.Kind = TokenKind::Caret;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '&':
                            State = State::Ampersand;
                            Result.Kind = TokenKind::Ampersand;

                            break;
                        case '|':
                            State = State::Pipe;

                            Result.Kind = TokenKind::Pipe;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '~':
                            State = State::Tilde;

                            Result.Kind = TokenKind::Tilde;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '<':
                            State = State::LessThan;

                            Result.Kind = TokenKind::LessThan;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '>':
                            State = State::GreaterThan;

                            Result.Kind = TokenKind::GreaterThan;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '=':
                            State = State::Equal;

                            Result.Kind = TokenKind::Equal;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '!':
                            State = State::Exclamation;

                            Result.Kind = TokenKind::Exclamation;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case '(':
                            Result.Kind = TokenKind::OpenParen;
                            Result.Loc.Column = this->Loc.Column;

                            this->Loc.Column += 1;
                            goto done;
                        case ')':
                            Result.Kind = TokenKind::CloseParen;
                            Result.Loc.Column = this->Loc.Column;

                            this->Loc.Column += 1;
                            goto done;
                        case '{':
                            Result.Kind = TokenKind::OpenCurlyBrace;
                            Result.Loc.Column = this->Loc.Column;

                            this->Loc.Column += 1;
                            goto done;
                        case '}':
                            Result.Kind = TokenKind::CloseCurlyBrace;
                            Result.Loc.Column = this->Loc.Column;

                            this->Loc.Column += 1;
                            goto done;
                        case '[':
                            Result.Kind = TokenKind::LeftSquareBracket;
                            Result.Loc.Column = this->Loc.Column;

                            this->Loc.Column += 1;
                            goto done;
                        case ']':
                            Result.Kind = TokenKind::RightSquareBracket;
                            Result.Loc.Column = this->Loc.Column;

                            this->Loc.Column += 1;
                            goto done;
                        case ',':
                            Result.Kind = TokenKind::Comma;
                            Result.Loc.Column = this->Loc.Column;

                            this->Loc.Column += 1;
                            goto done;
                        case '.':
                            State = State::Dot;

                            Result.Kind = TokenKind::Dot;
                            Result.Loc.Column = this->Loc.Column;

                            break;
                        case ':':
                            Result.Kind = TokenKind::Colon;
                            Result.Loc.Column = this->Loc.Column;

                            this->Loc.Column += 1;
                            goto done;
                        case ';':
                            Result.Kind = TokenKind::Semicolon;
                            Result.Loc.Column = this->Loc.Column;

                            this->Loc.Column += 1;
                            goto done;
                        case '?':
                            Result.Kind = TokenKind::QuestionMark;
                            Result.Loc.Column = this->Loc.Column;

                            this->Loc.Column += 1;
                            goto done;
                        default:
                            this->Diag.consume({
                                .Level = DiagnosticLevel::Error,
                                .Location = this->Loc,
                                .Message =
                                    std::format("Unrecognized character '{}'",
                                                Char)
                            });

                            Result.Kind = TokenKind::Invalid;
                            goto done;
                    }

                    break;
                case State::CharLiteral:
                    switch (Char) {
                        case '\\':
                            this->Loc.Index++;
                            break;
                        case '\'':
                            goto done;
                        default:
                            break;
                    }

                    break;
                case State::StringLiteral:
                    switch (Char) {
                        case '\\':
                            this->Loc.Index++;
                            break;
                        case '"':
                            goto done;
                        default:
                            break;
                    }

                    break;
                case State::IntegerLiteral:
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
                            Result.Kind = TokenKind::FloatLiteral;
                            goto done;
                        case 's':
                        case 'S':
                        case 'u':
                        case 'U':
                            Result.Kind = TokenKind::IntegerLiteralWithSuffix;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::IntegerLiteralWithSuffix: {
                    switch (Char) {
                        case '0'...'9':
                            break;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                }
                case State::FloatLiteral:
                    switch (Char) {
                        case '0'...'9':
                            break;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

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
                            this->Loc.Index--;
                            this->Loc.Column--;

                            Result.End.Index = this->Loc.Index;
                            const auto KeywordOpt =
                                KeywordToLexemeMap.keyFor(
                                    Result.getString(Text));

                            if (KeywordOpt.has_value()) {
                                Result.Kind = TokenKind::Keyword;
                            }

                            goto done;
                    }

                    break;
                case State::DotIdentifier:
                    switch (Char) {
                        case '0'...'9':
                        case 'a'...'z':
                        case 'A'...'Z':
                        case '_':
                            break;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::Plus:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::PlusEqual;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::Minus:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::MinusEqual;
                            goto done;
                        case '>':
                            Result.Kind = TokenKind::ThinArrow;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

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
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::Percent:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::PercentEqual;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::Slash:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::SlashEqual;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::Caret:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::CaretEqual;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

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
                            this->Loc.Index--;
                            this->Loc.Column--;

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
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::Tilde:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::TildeEqual;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::LessThan:
                    switch (Char) {
                        case '<':
                            State = State::ShiftLeft;
                            Result.Kind = TokenKind::ShiftLeft;

                            break;
                        case '=':
                            Result.Kind = TokenKind::LessThanOrEqual;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::GreaterThan:
                    switch (Char) {
                        case '>':
                            State = State::ShiftRight;
                            Result.Kind = TokenKind::ShiftRight;

                            break;
                        case '=':
                            Result.Kind = TokenKind::GreaterThanOrEqual;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::ShiftLeft:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::ShiftLeftEqual;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::ShiftRight:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::ShiftRightEqual;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::Equal:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::DoubleEqual;
                            goto done;
                        case '>':
                            Result.Kind = TokenKind::FatArrow;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::Exclamation:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::NotEqual;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::Dot:
                    switch (Char) {
                        case 'a'...'z':
                        case 'A'...'Z':
                        case '0'...'9':
                        case '_':
                            Result.Kind = TokenKind::DotIdentifier;
                            State = State::DotIdentifier;

                            break;
                        case '*':
                            Result.Kind = TokenKind::DotStar;
                            goto done;
                        case '.':
                            Result.Kind = TokenKind::DotDot;
                            State = State::DotDot;

                            break;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
                case State::DotDot:
                    switch (Char) {
                        case '.':
                            Result.Kind = TokenKind::DotDotDot;
                            goto done;
                        case '<':
                            Result.Kind = TokenKind::DotDotLessThan;
                            goto done;
                        case '>':
                            Result.Kind = TokenKind::DotDotGreaterThan;
                            goto done;
                        case '=':
                            Result.Kind = TokenKind::DotDotEqual;
                            goto done;
                        default:
                            this->Loc.Index--;
                            this->Loc.Column--;

                            goto done;
                    }

                    break;
            }

            if (Char == '\n') {
                this->Loc.Row++;
                this->CurrentLineInfo = {
                    .ByteOffset = this->Loc.Index
                };
            }

            this->Loc.Column++;
            if (this->Loc.Column > SourceLocation::ColumnLimit) {
                return std::pair(Token::invalid(), this->CurrentLineInfo);
            }
        }

    done:
        State = State::Start;

        Result.End.Row = this->Loc.Row;
        Result.End.Column = this->Loc.Column;
        Result.End.Index = this->Loc.Index;

        return std::pair(Result, this->CurrentLineInfo);
    }
}