import BinaryOperator from "./binary-operator";
export enum TokenKind {
  IntegerLiteral = "integer-literal",
  BinaryOperator = "binary-operator",
  OpenParen = "open-parenthesis",
  ClosedParen = "close-parenthesis",
}

export interface IntegerLiteralToken {
  kind: TokenKind.IntegerLiteral;
  literal: number;
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
