import BinaryOperator from "./binary-operator";
import { Token, TokenKind, ParenthesisOpenToken } from "./token";

const spaces: string[] = [" ", "\n", "\t", "\r", "\f"];

export default class Lexer {
  expr: string;
  index: number = -1;
  tokens: Token[] = [];

  constructor(expr: string) {
    this.expr = expr;
  }

  peek(): string | undefined {
    if (this.index + 1 >= this.expr.length) {
      return undefined;
    }

    return this.expr[this.index + 1];
  }

  consume(): string | undefined {
    this.index++;
    if (this.index >= this.expr.length) {
      return undefined;
    }

    return this.expr[this.index];
  }

  readNumber(firstChar: string): number {
    let number = 0;
    let char: string | undefined = firstChar;

    do {
      switch (true) {
        case char >= "0" && char <= "9":
          this.consume();

          number *= 10;
          number += char.charCodeAt(0) - "0".charCodeAt(0); // convert char to n

          break;
        default:
          return number;
      }
    } while ((char = this.peek()));

    return number;
  }

  lex(): Token[] {
    let char: string | undefined = undefined;
    while ((char = this.consume())) {
      switch (true) {
        case spaces.includes(char):
          // spaces
          // skip for now
          continue;
        case char >= "0" && char <= "9":
          this.tokens.push({
            kind: TokenKind.IntegerLiteral,
            literal: this.readNumber(char),
            loc: this.index,
          });

          break;
        case Object.values(BinaryOperator).includes(char as BinaryOperator): // figure something else out for this
          this.tokens.push({
            kind: TokenKind.BinaryOperator,
            op: char as BinaryOperator,
            loc: this.index,
          });

          break;
        case char == "(":
          this.tokens.push({
            kind: TokenKind.ParenthesisOpen,
            loc: this.index,
          });

          break;
        case char == ")":
          this.tokens.push({
            kind: TokenKind.ParenthesisClosed,
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
