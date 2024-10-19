/*
 * Lex/Tokenizer.cpp
 */

#include "Lex/Tokenizer.h"

namespace Lex {
    auto Tokenizer::next() noexcept -> Token {
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

                if (Index == Text.length()) {
                    Result = Token::eof();
                    break;
                }

                Diag.emitInternalError(
                    "Tokenizer::next(): Unexpected null char\n");

                return Token::invalid();
            }

            switch (State) {
                case State::Start:
                    Result.Loc.Index = Index - 1;
                    switch (Char) {
                        case '\n':
                            Row++;
                            Column = 0;

                            if (Row > SourceLocation::RowLimit) {
                                Diag.emitError(Result.Loc, "Line is too long");
                                return Token::invalid();
                            }

                            continue;
                        case ' ':
                        case '\t':
                        case '\r':
                            goto next;
                        case '\'':
                            State = State::CharLiteral;
                            Result.Kind = TokenKind::CharLiteral;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto next;
                        case '"':
                            State = State::StringLiteral;
                            Result.Kind = TokenKind::StringLiteral;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto next;
                        case 'a'...'z':
                        case 'A'...'Z':
                        case '_':
                            State = State::Identifier;
                            Result.Kind = TokenKind::Identifier;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto next;
                        case '0'...'9':
                            State = State::IntegerLiteral;
                            Result.Kind = TokenKind::IntegerLiteral;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto next;
                        case '+':
                            State = State::Plus;
                            Result.Kind = TokenKind::Plus;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto next;
                        case '-':
                            State = State::Minus;
                            Result.Kind = TokenKind::Minus;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto next;
                        case '*':
                            State = State::Star;
                            Result.Kind = TokenKind::Star;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto next;
                        case '%':
                            State = State::Percent;
                            Result.Kind = TokenKind::Percent;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto next;
                        case '/':
                            State = State::Slash;
                            Result.Kind = TokenKind::Slash;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto next;
                        case '^':
                            State = State::Caret;
                            Result.Kind = TokenKind::Caret;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            break;
                        case '&':
                            State = State::Ampersand;
                            Result.Kind = TokenKind::Ampersand;
                            break;
                        case '|':
                            State = State::Pipe;
                            Result.Kind = TokenKind::Pipe;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            break;
                        case '~':
                            State = State::Tilde;
                            Result.Kind = TokenKind::Tilde;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            break;
                        case '<':
                            State = State::LessThan;
                            Result.Kind = TokenKind::LessThan;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            break;
                        case '>':
                            State = State::GreaterThan;
                            Result.Kind = TokenKind::GreaterThan;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            break;
                        case '=':
                            State = State::Equal;
                            Result.Kind = TokenKind::Equal;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            break;
                        case '!':
                            State = State::Exclamation;
                            Result.Kind = TokenKind::Exclamation;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            break;
                        case '(':
                            Result.Kind = TokenKind::OpenParen;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto done;
                        case ')':
                            Result.Kind = TokenKind::CloseParen;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto done;
                        case '{':
                            Result.Kind = TokenKind::OpenCurlyBrace;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto done;
                        case '}':
                            Result.Kind = TokenKind::CloseCurlyBrace;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto done;
                        case '[':
                            Result.Kind = TokenKind::LeftSquareBracket;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto done;
                        case ']':
                            Result.Kind = TokenKind::RightSquareBracket;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto done;
                        case ',':
                            Result.Kind = TokenKind::Comma;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto done;
                        case '.':
                            State = State::Dot;
                            Result.Kind = TokenKind::Dot;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            break;
                        case ':':
                            Result.Kind = TokenKind::Colon;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto done;
                        case ';':
                            Result.Kind = TokenKind::Semicolon;

                            Result.Loc.Row = Row;
                            Result.Loc.Column = Column;

                            goto done;
                        default:
                            Diag.emitError(SourceLocation::invalid(),
                                           "Unrecognized token: %c\n",
                                           Char);

                            Result.Kind = TokenKind::Invalid;
                            goto done;
                    }

                    break;
                case State::CharLiteral:
                    switch (Char) {
                        case '\\':
                            Index++;
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
                            Index++;
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
                        default:
                            Index--;
                            Column--;

                            goto done;
                    }

                    break;
                case State::FloatLiteral:
                    switch (Char) {
                        case '0'...'9':
                            break;
                        default:
                            Index--;
                            Column--;

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
                            Column--;

                            Result.End.Index = Index;
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
                            Index--;
                            Column--;

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
                            Column--;

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
                            Index--;
                            Column--;

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
                            Column--;

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
                            Column--;

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
                            Column--;

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
                            Column--;

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
                            Column--;

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
                            Column--;

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
                            Column--;

                            goto done;
                    }

                    break;
                case State::LessThan:
                    switch (Char) {
                        case '<':
                            State = State::ShiftLeft;
                            Result.Kind = TokenKind::ShiftLeft;

                            goto next;
                        case '=':
                            Result.Kind = TokenKind::LessThanOrEqual;
                            goto done;
                        default:
                            Index--;
                            Column--;

                            goto done;
                    }

                    break;
                case State::GreaterThan:
                    switch (Char) {
                        case '>':
                            State = State::ShiftRight;
                            Result.Kind = TokenKind::ShiftRight;

                            goto next;
                        case '=':
                            Result.Kind = TokenKind::GreaterThanOrEqual;
                            goto done;
                        default:
                            Index--;
                            Column--;

                            goto done;
                    }

                    break;
                case State::ShiftLeft:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::ShiftLeftEqual;
                            goto done;
                        default:
                            Index--;
                            Column--;

                            goto done;
                    }

                    break;
                case State::ShiftRight:
                    switch (Char) {
                        case '=':
                            Result.Kind = TokenKind::ShiftRightEqual;
                            goto done;
                        default:
                            Index--;
                            Column--;

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
                            Index--;
                            Column--;

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
                            Column--;

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

                            goto next;
                        default:
                            Index--;
                            Column--;

                            goto done;
                    }

                    break;
            }

        next:
            if (Char == '\n') {
                Row++;
            }

            Column++;
            if (Column > SourceLocation::ColumnLimit) {
                Diag.emitError(SourceLocation::forLine(Row),
                               "Line is too long");
                return Token::invalid();
            }
        }

    done:
        State = State::Start;

        Result.End.Row = Row;
        Result.End.Column = Column + 1;
        Result.End.Index = Index;

        return Result;
    }
}