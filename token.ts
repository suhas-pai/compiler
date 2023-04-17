import BinaryOperator from "./binary-operator";
export enum TokenKind {
  IntegerLiteral = "integer-literal",
  BinaryOperator = "binary-operator",
  ParenthesisOpen = "parenthesis-open",
  ParenthesisClosed = "parenthesis-closed",
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
  kind: TokenKind.ParenthesisOpen;
}

export interface ParenthesisClosedToken {
  kind: TokenKind.ParenthesisClosed;
}

export type Token = (
  | IntegerLiteralToken
  | BinaryOperatorToken
  | ParenthesisOpenToken
  | ParenthesisClosedToken
) & {
  loc: number;
};
