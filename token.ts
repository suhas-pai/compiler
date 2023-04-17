import BinaryOperator from "./binary-operator";
import ParenthesisOperator from "./ast/parenthesis";

export enum TokenKind {
  IntegerLiteral = "integer-literal",
  BinaryOperator = "binary-operator",
  ParenthesisOperator = "parenthesis-operator",
}

export interface IntegerLiteralToken {
  kind: TokenKind.IntegerLiteral;
  literal: Number;
}

export interface BinaryOperatorToken {
  kind: TokenKind.BinaryOperator;
  op: BinaryOperator;
}

export interface ParenthesisOperatorToken {
  kind: TokenKind.ParenthesisOperator;
  op: ParenthesisOperator;
}

export type Token = (IntegerLiteralToken | BinaryOperatorToken | ParenthesisOperatorToken) & {
  loc: number;
};
