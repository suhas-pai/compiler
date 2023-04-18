import BinaryOperator from "./binary-operator";
import { Token, TokenKind, ParenthesisOpenToken } from "./token";

const spaces: string[] = [" ", "\n", "\t", "\r", "\f"];
const separators: string[] = [...spaces, "(", ")", "[", "]"];

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

  readNumber(firstChar: string): number {
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
          // spaces
          // skip for now
          continue;
        case this.isdigit(char):
          this.tokens.push({
            kind: TokenKind.IntegerLiteral,
            literal: this.readNumber(char),
            loc: this.index,
          });

          break;
        case char == "-": {
          const prev = this.rpeek();
          if (
            this.isdigit(this.peek()) &&
            (prev == undefined || separators.includes(prev))
          ) {
            const firstChar = this.consume();
            this.tokens.push({
              kind: TokenKind.IntegerLiteral,
              literal: -1 * this.readNumber(firstChar),
              loc: this.index,
            });
          } else {
            this.handleBinOp(char);
          }

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
        case char == "+":
        case char == "%":
        case char == "/":
          this.handleBinOp(char);
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
        default:
          throw `Not an allowed character ${char}`;
      }
    }
    return this.tokens;
  }
}
