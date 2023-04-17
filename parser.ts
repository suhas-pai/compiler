import AST from "./ast";

import BinaryOperation from "./ast/binary-operation";
import IntegerLiteral from "./ast/integer-literal";
import ASTNode from "./ast/node";

import { Token, TokenKind } from "./token";
import BinaryOperator from "./binary-operator";

export class Parser {
  tokens: Token[];
  index: number = -1;

  constructor(tokens: Token[]) {
    this.tokens = tokens;
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

  parse(ast: AST): ASTNode {
    let token: Token | null;
    while ((token = this.next())) {
      switch (token.kind) {
        case TokenKind.IntegerLiteral: {
          const node = new IntegerLiteral(token);
          if (ast.root == null) {
            ast.root = node;
            continue;
          }

          // If we have a number w/o a preceding operation (e.g "2 2") or
          // If we have a number after a full binary operation (e.g "2 + 2 3").
          if (
            ast.currentOperation == null ||
            ast.currentOperation.right() != null
          ) {
            throw `Got number ${token.literal}, expected an operation`;
          }

          ast.currentOperation.setRight(node);
          break;
        }
        case TokenKind.BinaryOperator: {
          const node = new BinaryOperation(token);
          if (ast.root == null) {
            throw `Got binary-operator ${token.op} w/o a preceding number`;
          }

          // When we have only a single element in the tree - a left-number.
          if (ast.currentOperation == null) {
            ast.currentOperation = node;
            ast.currentOperation.setLeft(ast.root);
            ast.root = ast.currentOperation;

            continue;
          }

          const curOp = ast.currentOperation.token.op;
          if (Parser.precedence(curOp) > Parser.precedence(token.op)) {
            // Create a new root for this binary operation, that sits above
            // the current binary operation.
            // The current (now previous) binary opeation will be the left child
            // of this new operation.
            ast.currentOperation = node;
            ast.currentOperation.setLeft(ast.root);
            ast.root = ast.currentOperation;
          } else {
            // Take the most-recently-seen number (the right-number of the
            // current operation), and make it the left-number of this operation
            // that has a higher precedence.
            // The current (now previous) operation's right-number will be the
            // result of this new, higher-precedenced operation.

            node.setLeft(ast.currentOperation.right());

            ast.currentOperation.setRight(node);
            ast.currentOperation = node;
          }

          break;
        }
        default:
          throw "Got unrecognized token";
      }
    }

    return ast.root;
  }

  static precedence(operator: BinaryOperator): Number {
    switch (operator) {
      case BinaryOperator.plus:
      case BinaryOperator.minus:
        return 0;
      case BinaryOperator.div:
      case BinaryOperator.mult:
        return 1;
      default:
        throw "Illegal operator type (How did this happen??)";
    }
  }
}
