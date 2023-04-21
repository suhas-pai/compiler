import { KeywordKind } from "./keywords";
import { BinaryOperator, UnaryOperator } from "./operators";

export enum TokenKind {
  IntegerLiteral = "integer-literal",
  StringLiteral = "string-literal",
  BinaryOperator = "binary-operator",
  UnaryOperator = "unary-operator",
  OpenParen = "open-parenthesis",
  ClosedParen = "close-parenthesis",
  KeywordIdentifier = "KeywordIdentifier",
  Identifier = "identifier",
  Equal = "equal-sign",
  EndLine = "end-line",
}

export interface IntegerLiteralToken {
  kind: TokenKind.IntegerLiteral;
  loc: number;
  literal: number;
}

export interface StringLiteralToken {
  kind: TokenKind.StringLiteral;
  loc: number;
  literal: string;
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

export interface KeywordIdentifierToken {
  kind: TokenKind.KeywordIdentifier;
  loc: number;
  keyword: KeywordKind;
}

export interface IdentifierToken {
  kind: TokenKind.Identifier;
  loc: number;
  name: string;
}

export interface EqualToken {
  kind: TokenKind.Equal;
  loc: number;
}

export interface EndLineToken {
  kind: TokenKind.EndLine;
  loc: number;
}

export type Token =
  | IntegerLiteralToken
  | StringLiteralToken
  | BinaryOperatorToken
  | UnaryOperatorToken
  | ParenthesisOpenToken
  | ParenthesisClosedToken
  | KeywordIdentifierToken
  | IdentifierToken
  | EqualToken
  | EndLineToken
