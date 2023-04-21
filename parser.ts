import AST from "./ast/base";
import BinaryOperation from "./ast/nodes/binary-operation";
import GlobalScope from "./ast/nodes/global-scope";
import IntegerLiteral from "./ast/nodes/integer-literal";
import ParenExpr from "./ast/nodes/paren-expr";
import UnaryOperation from "./ast/nodes/unary-operation";
import VariableDecl from "./ast/nodes/variable-decl";

import ASTNode from "./ast/node";

import { KeywordKind } from "./keywords";
import { AssocKind, BinOperatorInfoMap, BinaryOperator } from "./operators";
import {
  IntegerLiteralToken,
  Token,
  TokenKind,
  BinaryOperatorToken,
} from "./token";
import StringLiteral from "./ast/nodes/string-literal";

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
        case TokenKind.StringLiteral:
          if (root) {
            root.addChild(new StringLiteral(token));
            return root;
          }
          
          return new StringLiteral(token);
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
            }

            return root;
          }

          throw `Error`;
        case TokenKind.UnaryOperator:
          root = new UnaryOperation(token);
          break;
        case TokenKind.OpenParen: {
          const parenExpr = new ParenExpr();
          parenExpr.addChild(this.parseExpression(TokenKind.ClosedParen));

          if (root) {
            root.addChild(parenExpr);
            return root;
          }

          return parenExpr;
        }
        default:
          throw `Unexpected token: ${token.kind}`;
      }
    }

    return root;
  }

  private parseRHSOfExpression(
    lhs: ASTNode,
    minPrecedence: number,
    endTokenKind?: TokenKind
  ): ASTNode {
    // Not necessarily, but every successive root is a BinaryOperation
    let root: BinaryOperation = lhs as BinaryOperation;
    let nextToken = this.peek();

    while (nextToken?.kind == TokenKind.BinaryOperator) {
      const opTok = this.consume() as BinaryOperatorToken; // = lookahead
      const opInfo = BinOperatorInfoMap.get(opTok.op);

      if (opInfo.precedence < minPrecedence) {
        break;
      }

      let rhs: ASTNode;
      if (opInfo.assoc == AssocKind.Right) {
        rhs = this.parseRHSOfExpression(this.parseLHS(), 0, endTokenKind);
      } else {
        rhs = this.parseLHS();
      }

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
      while (nextToken?.kind == TokenKind.BinaryOperator) {
        const nextTokenBinInfo = BinOperatorInfoMap.get(nextToken.op);
        if (
          nextTokenBinInfo.precedence <= opInfo.precedence &&
          (nextTokenBinInfo.assoc != AssocKind.Right ||
            nextTokenBinInfo.precedence != opInfo.precedence)
        ) {
          break;
        }

        const nextMinPrecedence =
          nextTokenBinInfo.precedence +
          (nextTokenBinInfo.assoc != AssocKind.Right ? 1 : 0);

        rhs = this.parseRHSOfExpression(rhs, nextMinPrecedence);
        nextToken = this.peek(TokenKind.BinaryOperator);

        currentOperation.setRight(rhs);
        currentOperation = rhs as BinaryOperation;
      }
    }

    return root;
  }

  parseExpression(endTokenKind?: TokenKind) {
    return this.parseRHSOfExpression(this.parseLHS(), 0, endTokenKind);
  }

  parseVarDecl(): ASTNode {
    const nameToken = this.consume();
    if (!nameToken) {
      throw `Expected an identifier`;
    }

    if (nameToken.kind != TokenKind.Identifier) {
      if (nameToken.kind == TokenKind.KeywordIdentifier) {
        throw `${nameToken.keyword} cannot be used as a variable name`;
      }

      throw `Expected an identifier for variable name`;
    }

    const varDecl = new VariableDecl(nameToken);
    const nextToken = this.consume(TokenKind.Equal);

    varDecl.setExpr(this.parseExpression());
    return varDecl;
  }

  parseStatement(): ASTNode {
    const token = this.peek();
    if (!token) {
      throw `Expected an identifier`;
    }

    switch (token.kind) {
      case TokenKind.KeywordIdentifier:
        switch (token.keyword) {
          case KeywordKind.Let:
            this.consume();
            return this.parseVarDecl();
          default:
            throw `Unrecognized keyword ${token.keyword}`;
        }
      default:
        return this.parseExpression();
    }
  }

  parse(ast: AST): ASTNode {
    ast.setRoot(new GlobalScope());
    ast.root.addChild(this.parseStatement());

    return ast.root;
  }
}
