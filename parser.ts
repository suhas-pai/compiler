import AST from "./ast/base";
import BinaryOperation from "./ast/nodes/binary-operation";
import GlobalScope from "./ast/nodes/global-scope";
import IntegerLiteral from "./ast/nodes/integer-literal";
import ParenExpr from "./ast/nodes/paren-expr";
import UnaryOperation from "./ast/nodes/unary-operation";

import ASTNode from "./ast/node";

import { BinaryOperator, binOpPrecedence } from "./operators";
import {
  IntegerLiteralToken,
  Token,
  TokenKind,
  BinaryOperatorToken,
} from "./token";

export default class Parser {
  tokens: Token[];
  index: number = -1;

  constructor(tokens: Token[]) {
    this.tokens = tokens;
  }

  peek(kind?: TokenKind): Token | null {
    if (this.index + 1 >= this.tokens.length) {
      return null;
    }

    const token = this.tokens[this.index + 1];
    if (kind && token.kind != kind) {
      return null;
    }

    return token;
  }

  rpeek(kind?: TokenKind): Token | null {
    if (this.index == 0) {
      return null;
    }

    const token = this.tokens[this.index - 1];
    if (kind && token.kind != kind) {
      return null;
    }

    return token;
  }

  current(kind?: TokenKind): Token | null {
    if (this.index >= this.tokens.length) {
      return null;
    }

    const token = this.tokens[this.index];
    if (kind && token.kind != kind) {
      throw `Expected a ${kind}, got ${token.kind})`;
    }

    return token;
  }

  consume(kind?: TokenKind): Token | null {
    this.index += 1;
    return this.current(kind);
  }

  private parseLHS(): ASTNode {
    let token: Token;
    let root: ASTNode;
    while ((token = this.consume())) {
      switch (token.kind) {
        case TokenKind.IntegerLiteral:
          if (root) {
            root.addChild(new IntegerLiteral(token));
            return root;
          }

          return new IntegerLiteral(token);
        case TokenKind.BinaryOperator:
          if (
            token.op == BinaryOperator.minus &&
            this.peek().kind == TokenKind.IntegerLiteral
          ) {
            // Remove the minus sign token, and treat the minus-sign +
            // integer-literal as one token.

            this.tokens.splice(this.index, 1);
            const intTok = this.current() as IntegerLiteralToken;

            intTok.loc = token.loc;
            intTok.literal *= -1;

            if (root) {
              root.addChild(new IntegerLiteral(intTok));
              return root;
            }

            return root;
          }

          throw `Error`;
        case TokenKind.UnaryOperator:
          root = new UnaryOperation(token);
          break;
        case TokenKind.OpenParen: {
          const parenExpr = new ParenExpr();
          parenExpr.addChild(
            this.parseExpr(this.parseLHS(), 0, TokenKind.ClosedParen)
          );

          if (root) {
            root.addChild(parenExpr);
            return root;
          }

          return parenExpr;
        }
      }
    }

    return root;
  }

  private parseExpr(
    lhs: ASTNode,
    minPrecedence: number,
    endTokenKind?: TokenKind
  ): ASTNode {
    let nextToken = this.peek();
    // Not actually, but every successive prevNode is a BinaryOperation
    let root: BinaryOperation = lhs as BinaryOperation;

    while (
      nextToken?.kind == TokenKind.BinaryOperator &&
      binOpPrecedence(nextToken?.op) >= minPrecedence
    ) {
      const opTok = this.consume() as BinaryOperatorToken; // = lookahead
      let rhs = this.parseLHS();

      root = new BinaryOperation(opTok, root, rhs);
      nextToken = this.peek();

      if (!nextToken) {
        break;
      }

      if (nextToken.kind == endTokenKind) {
        this.consume();
        break;
      }

      let currentOperation = root;
      while (
        nextToken?.kind == TokenKind.BinaryOperator &&
        binOpPrecedence((nextToken as BinaryOperatorToken).op) >
          binOpPrecedence(opTok.op)
      ) {
        rhs = this.parseExpr(rhs, binOpPrecedence(opTok.op) + 1);
        nextToken = this.peek(TokenKind.BinaryOperator);

        currentOperation.setRight(rhs);
        currentOperation = rhs as BinaryOperation;
      }
    }

    return root;
  }

  parse(ast: AST): ASTNode {
    ast.setRoot(new GlobalScope());
    ast.root.addChild(this.parseExpr(this.parseLHS(), 0));

    return ast.root;
  }
}
