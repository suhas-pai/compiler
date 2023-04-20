import { BinaryOperator, UnaryOperator } from "./operators";

export enum TokenKind {
  IntegerLiteral = "integer-literal",
  BinaryOperator = "binary-operator",
  UnaryOperator = "unary-operator",
  OpenParen = "open-parenthesis",
  ClosedParen = "close-parenthesis",
}

export interface IntegerLiteralToken {
  kind: TokenKind.IntegerLiteral;
  loc: number;
  literal: number;
}

export interface BinaryOperatorToken {
  kind: TokenKind.BinaryOperator;
  loc: number;
  op: BinaryOperator;
}

export interface UnaryOperatorToken {
  kind: TokenKind.UnaryOperator;
  loc: number;
  op: UnaryOperator;
}

export interface ParenthesisOpenToken {
  kind: TokenKind.OpenParen;
  loc: number;
}

export interface ParenthesisClosedToken {
  kind: TokenKind.ClosedParen;
  loc: number;
}

export type Token =
  | IntegerLiteralToken
  | BinaryOperatorToken
  | UnaryOperatorToken
  | ParenthesisOpenToken
  | ParenthesisClosedToken;
