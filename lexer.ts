import { KeywordKind } from "./keywords";
import { BinaryOperator, UnaryOperator } from "./operators";
import { Token, TokenKind } from "./token";

const spaces: string[] = [" ", "\n", "\t", "\r", "\f"];

export default class Lexer {
  expr: string;
  index: number = -1;
  tokens: Token[] = [];

  constructor(expr: string) {
    this.expr = expr;
  }

  rpeek(): string | undefined {
    if (this.index == 0) {
      return undefined;
    }

    return this.expr[this.index - 1];
  }

  peek(): string | undefined {
    if (this.index + 1 >= this.expr.length) {
      return undefined;
    }

    return this.expr[this.index + 1];
  }

  consume(amount: number = 1): string | undefined {
    this.index += amount;
    if (this.index >= this.expr.length) {
      return undefined;
    }

    return this.expr[this.index];
  }

  private todigit(char: string, base: number): number | undefined {
    switch (true) {
      case char >= "0" && char <= "9": {
        const digit = char.charCodeAt(0) - "0".charCodeAt(0);
        if (digit >= base) {
          throw `Invalid digit ${digit} for number with base ${base}`;
        }

        return digit;
      }
      case char >= "a" && char <= "z": {
        const digit = 10 + (char.charCodeAt(0) - "a".charCodeAt(0));
        if (digit >= base) {
          throw `Invalid character ${digit} for number with base ${base}`;
        }

        return digit;
      }
      case char >= "A" && char <= "Z": {
        const digit = 10 + (char.charCodeAt(0) - "A".charCodeAt(0));
        if (digit >= base) {
          throw `Invalid character ${digit} for number with base ${base}`;
        }

        return digit;
      }
    }

    return undefined;
  }

  private readString(): [string, number] {
    let newString = "";
    let tokenLength = 1;
    let char: string;

    while ((char = this.consume())) {
      tokenLength++;
      switch (char) {
        case '"':
          return [newString, tokenLength];
        case "\\":
          char = this.consume();
          if (!char) {
            throw `Unexpected end of string`;
          }

          switch (char) {
            case "\0":
              throw `Invalid escape sequence \\0`;
            case "a":
              newString += "a";
              break;
            case "b":
              newString += "\b";
              break;
            case "f":
              newString += "\f";
              break;
            case "n":
              newString += "\n";
              break;
            case "r":
              newString += "\r";
              break;
            case "t":
              newString += "\t";
              break;
            case "v":
              newString += "\v";
              break;
            case "\\":
              newString += "\\";
              break;
            case "'":
              newString += "'";
              break;
            case '"':
              newString += '"';
              break;
            default:
              throw `Invalid escape sequence \\${char}`;
          }

          break;
      }

      newString += char;
    }

    throw `String reaches to end of input`;
  }

  private readNumber(firstChar: string): number {
    let char: string | undefined = firstChar;
    let base = 10;

    if (firstChar == "0") {
      const char = this.peek();
      switch (char) {
        case "b":
        case "B":
          base = 2;
          firstChar = this.consume(2);
          break;
        case "o":
        case "O":
          base = 8;
          firstChar = this.consume(2);
          break;
        case "x":
        case "X":
          base = 16;
          firstChar = this.consume(2);
          break;
      }
    }

    let number = this.todigit(firstChar, base);
    while ((char = this.peek())) {
      const digit = this.todigit(char, base);
      if (digit == undefined) {
        return number;
      }

      if (digit >= base) {
        throw `Invalid character ${digit} for base ${base}`;
      }

      this.consume();

      number *= base;
      number += digit;
    }

    return number;
  }

  private readIdentifier(): string {
    let start = this.index;
    let end = start + 1;

    let token: string = this.peek();
    while ((token = this.peek())) {
      if (
        (token[0] >= "a" && token[0] <= "z") ||
        (token[0] >= "A" && token[0] <= "Z") ||
        (token[0] >= "0" && token[0] <= "9") ||
        token == "_"
      ) {
        this.consume();
        end += 1;

        continue;
      }

      break;
    }

    return this.expr.substring(start, end);
  }

  private isdigit(str: string): boolean {
    return str[0] >= "0" && str[0] <= "9";
  }

  private handleBinOp(op: string): void {
    this.tokens.push({
      kind: TokenKind.BinaryOperator,
      op: op as BinaryOperator,
      loc: this.index,
    });
  }

  lex(): Token[] {
    let char: string | undefined = undefined;
    while ((char = this.consume())) {
      switch (true) {
        case spaces.includes(char):
          continue;
        case this.isdigit(char):
          this.tokens.push({
            kind: TokenKind.IntegerLiteral,
            loc: this.index,
            literal: this.readNumber(char),
          });

          break;
        case char == '"': {
          const index = this.index;
          const [string, length] = this.readString();

          this.tokens.push({
            kind: TokenKind.StringLiteral,
            literal: string,
            loc: index,
            length: length,
          });

          break;
        }
        case char == "*":
          if (this.peek() == "*") {
            this.consume();
            this.handleBinOp("**");
          } else {
            this.handleBinOp(char);
          }

          break;
        case char == "-":
        case char == "+":
        case char == "%":
        case char == "/":
          this.handleBinOp(char);
          break;
        case char == "!":
        case char == "~":
          this.tokens.push({
            kind: TokenKind.UnaryOperator,
            op: char as UnaryOperator,
            loc: this.index,
          });
          break;
        case char == "=":
          this.tokens.push({
            kind: TokenKind.Equal,
            loc: this.index,
          });

          break;
        case char == "(":
          this.tokens.push({
            kind: TokenKind.OpenParen,
            loc: this.index,
          });

          break;
        case char == ")":
          this.tokens.push({
            kind: TokenKind.ClosedParen,
            loc: this.index,
          });

          break;
        case char == ";":
          this.tokens.push({
            kind: TokenKind.EndLine,
            loc: this.index,
          });
          break;
        case char >= "a" && char <= "z":
        case char >= "A" && char <= "Z": {
          const identifier = this.readIdentifier();
          if (Object.values(KeywordKind).includes(identifier as KeywordKind)) {
            this.tokens.push({
              kind: TokenKind.KeywordIdentifier,
              loc: this.index,
              keyword: identifier as KeywordKind,
            });
          } else {
            this.tokens.push({
              kind: TokenKind.Identifier,
              loc: this.index,
              name: identifier,
            });
          }

          break;
        }
        default:
          throw `Not an allowed character ${char}`;
      }
    }
    return this.tokens;
  }
}
