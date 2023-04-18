import BinaryOperator from "./binary-operator";
export enum TokenKind {
  IntegerLiteral = "integer-literal",
  BinaryOperator = "binary-operator",
  OpenParen = "parenthesis-open",
  ClosedParen = "parenthesis-closed",
}

export interface IntegerLiteralToken {
  kind: TokenKind.IntegerLiteral;
  literal: Number;
}

export interface BinaryOperatorToken {
  kind: TokenKind.BinaryOperator;
  op: BinaryOperator;
}

export interface ParenthesisOpenToken {
  kind: TokenKind.OpenParen;
}

export interface ParenthesisClosedToken {
  kind: TokenKind.ClosedParen;
}

export type Token = (
  | IntegerLiteralToken
  | BinaryOperatorToken
  | ParenthesisOpenToken
  | ParenthesisClosedToken
) & {
  loc: number;
};
