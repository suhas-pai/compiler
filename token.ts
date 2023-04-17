import BinaryOperator from "./binary-operator";

export enum TokenKind {
  IntegerLiteral = "integer-literal",
  BinaryOperator = "binary-operator",
}

const spaces: string[] = [" ", "\n", "\t", "\r", "\f"];

export interface IntegerLiteralToken {
  kind: TokenKind.IntegerLiteral;
  num: Number;
}
export interface BinaryOperatorToken {
  kind: TokenKind.BinaryOperator;
  op: BinaryOperator;
}

export type Token = IntegerLiteralToken | BinaryOperatorToken;
