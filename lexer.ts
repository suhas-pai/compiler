import BinaryOperator from "./binary-operator";
import { Token, TokenKind } from "./token";

const spaces: string[] = [" ", "\n", "\t", "\r", "\f"];

export class Lexer {
  static readNumber(expr: string, i: number): [number, number] {
    let number = 0;
    for (; i < expr.length; i++) {
      const char = expr.charAt(i);
      switch (true) {
        case char >= "0" && char <= "9":
          number *= 10;
          number += char.charCodeAt(0) - "0".charCodeAt(0); // convert char to n
          break;
        case spaces.includes(char):
        case Object.values(BinaryOperator).includes(char as BinaryOperator):
          return [number, i];
        default:
          throw "Bad bad character!";
      }
    }
    return [number, i];
  }

  static lex(expr: string): Token[] {
    const tokens: Token[] = [];
    for (let i = 0; i < expr.length; i++) {
      const char = expr.charAt(i);
      switch (true) {
        case spaces.includes(char):
          // spaces
          // skip for now
          continue;
        case char >= "0" && char <= "9":
          // digits
          const [num, index] = this.readNumber(expr, i);
          tokens.push({
            kind: TokenKind.IntegerLiteral,
            num: num,
          });

          i = index;
          break;
        case Object.values(BinaryOperator).includes(char as BinaryOperator): // figure something else out for this
          tokens.push({
            kind: TokenKind.BinaryOperator,
            op: char as BinaryOperator,
          });

          break;
        default:
          throw `Not an allowed character ${char}`;
      }
    }
    return tokens;
  }
}
