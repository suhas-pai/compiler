import AST from "./ast/base";
import BinaryOperation from "./ast/nodes/binary-operation";
import IntegerLiteral from "./ast/nodes/integer-literal";

import ASTNode from "./ast/node";

import BinaryOperator from "./binary-operator";
import { Token, TokenKind } from "./token";
import ParenExpr from "./ast/nodes/paren-expr";
import GlobalScope from "./ast/nodes/global-scope";
import { BinaryOperatorToken } from "./token";

function binOpPrecedence(operator: BinaryOperator): number {
  switch (operator) {
    case BinaryOperator.plus:
    case BinaryOperator.minus:
      return 0;
    case BinaryOperator.mult:
    case BinaryOperator.modulo:
    case BinaryOperator.div:
      return 1;
    case BinaryOperator.power:
      return 2;
    default:
      throw "Illegal operator type (How did this happen??)";
  }
}

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

  consume(kind?: TokenKind): Token | null {
    this.index += 1;
    if (this.index >= this.tokens.length) {
      return null;
    }

    const token = this.tokens[this.index];
    if (kind && token.kind != kind) {
      throw `Expected a ${kind}, got ${token.kind})`;
    }

    return token;
  }

  private parsePrimary(): ASTNode {
    const token = this.consume();
    switch (token.kind) {
      case TokenKind.IntegerLiteral:
        return new IntegerLiteral(token);
      case TokenKind.BinaryOperator:
        throw `Error`;
      case TokenKind.OpenParen: {
        const parenExpr = new ParenExpr();
        parenExpr.addChild(
          this.parseExpr(this.parsePrimary(), 0, TokenKind.ClosedParen)
        );

        return parenExpr;
      }
    }
  }

  private parseExpr(
    lhs: ASTNode,
    minPrecedence: number,
    endTokenKind?: TokenKind
  ): ASTNode {
    let lookahead = this.peek();
    // Not actually, but every successive prevNode is a BinaryOperation
    let root: BinaryOperation = lhs as BinaryOperation;

    while (
      lookahead != null &&
      lookahead.kind == TokenKind.BinaryOperator &&
      binOpPrecedence(lookahead.op) >= minPrecedence
    ) {
      let opTok = this.consume() as BinaryOperatorToken; // = lookahead
      let rhs = this.parsePrimary();

      root = new BinaryOperation(opTok, root, rhs);
      lookahead = this.peek();

      if (!lookahead) {
        break;
      }

      if (lookahead.kind == endTokenKind) {
        this.consume();
        break;
      }

      let currentOperation = root;
      while (
        lookahead?.kind == TokenKind.BinaryOperator &&
        binOpPrecedence((lookahead as BinaryOperatorToken).op) >
          binOpPrecedence(opTok.op)
      ) {
        rhs = this.parseExpr(rhs, binOpPrecedence(opTok.op) + 1);
        lookahead = this.peek(TokenKind.BinaryOperator);

        currentOperation.setRight(rhs);
        currentOperation = rhs as BinaryOperation;
      }
    }

    return root;
  }

  parse(ast: AST): ASTNode {
    ast.setRoot(new GlobalScope());
    ast.root.addChild(this.parseExpr(this.parsePrimary(), 0));

    return ast.root;
  }
}
