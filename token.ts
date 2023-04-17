import BinaryOperator from "./binary-operator";

export enum TokenKind {
  IntegerLiteral = "integer-literal",
  BinaryOperator = "binary-operator",
}

export interface IntegerLiteralToken {
  kind: TokenKind.IntegerLiteral;
  num: Number;
}

export interface BinaryOperatorToken {
  kind: TokenKind.BinaryOperator;
  op: BinaryOperator;
}

export type Token = IntegerLiteralToken | BinaryOperatorToken;
