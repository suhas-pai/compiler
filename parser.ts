import AST from "./ast/base";
import BinaryOperation from "./ast/nodes/binary-operation";
import IntegerLiteral from "./ast/nodes/integer-literal";

import ASTNode from "./ast/node";

import BinaryOperator from "./binary-operator";
import { Token, TokenKind } from "./token";
import ParenExpr from "./ast/nodes/paren-expr";
import GlobalScope from "./ast/nodes/global-scope";

function binOpPrecedence(operator: BinaryOperator): Number {
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

  rpeek(kind?: TokenKind): Token | null {
    if (this.index == 0) {
      return null;
    }

    const token = this.tokens[this.index - 1];
    if (kind && token.kind != kind) {
      throw `Expected a ${kind}, got ${token.kind})`;
    }

    return token;
  }

  next(kind?: TokenKind): Token | null {
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

  private parseInternal(ast: AST, endTokenKind?: TokenKind): ASTNode {
    let parseRoot: ASTNode | null = null;
    let currentOperation: BinaryOperation | null = null;
    let token: Token | null;

    while ((token = this.next())) {
      if (token.kind == endTokenKind) {
        if (currentOperation != null && currentOperation.right() == null) {
          throw `Expected expression, found ${endTokenKind} instead`;
        }

        break;
      }

      switch (token.kind) {
        case TokenKind.IntegerLiteral: {
          const node = new IntegerLiteral(token);
          if (parseRoot == null) {
            parseRoot = node;
            continue;
          }

          // If we have a number w/o a preceding operation (e.g "2 2") or
          // If we have a number after a full binary operation (e.g "2 + 2 3").
          if (currentOperation == null || currentOperation.right() != null) {
            throw `Got number ${token.literal}, expected an operation`;
          }

          currentOperation.setRight(node);
          break;
        }
        case TokenKind.BinaryOperator: {
          const node = new BinaryOperation(token);
          if (parseRoot == null) {
            throw `Got binary-operator ${token.op} w/o a preceding number`;
          }

          // When we have only a single element in the tree - a left-number.
          if (currentOperation == null) {
            currentOperation = node;
            currentOperation.setLeft(parseRoot);
            parseRoot = currentOperation;

            continue;
          }

          // We got two binary-operations one after another.
          if (currentOperation.right() == null) {
            throw "Unexpected token: operation, expected expression";
          }

          const curOp = currentOperation.token.op;
          if (binOpPrecedence(curOp) > binOpPrecedence(token.op)) {
            // Create a new root for this binary operation, that sits above
            // the current binary operation.
            // The current (now previous) binary opeation will be the left child
            // of this new operation.
            currentOperation = node;
            currentOperation.setLeft(parseRoot);
            parseRoot = currentOperation;
          } else {
            // Take the most-recently-seen number (the right-number of the
            // current operation), and make it the left-number of this operation
            // that has a higher precedence.
            // The current (now previous) operation's right-number will be the
            // result of this new, higher-precedenced operation.

            node.setLeft(currentOperation.right());

            currentOperation.setRight(node);
            currentOperation = node;
          }

          break;
        }
        case TokenKind.OpenParen: {
          const node = new ParenExpr();
          if (parseRoot == null) {
            parseRoot = node;
          } else {
            parseRoot.addChild(node);
          }

          node.addChild(this.parseInternal(ast, TokenKind.ClosedParen));
          break;
        }
        case TokenKind.ClosedParen:
          throw "Unexpected token: ')'";
        default:
          throw "Got unrecognized token";
      }
    }

    if (parseRoot == null) {
      throw `Expected expression`;
    }

    return parseRoot;
  }

  parse(ast: AST): ASTNode {
    ast.setRoot(new GlobalScope());
    ast.root.addChild(this.parseInternal(ast));

    return ast.root;
  }
}
