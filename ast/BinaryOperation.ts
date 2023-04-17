import ASTNode from "./Node";
import ASTNodeKind from "./Kind";
import { BinaryOperatorToken } from "../token";
import BinaryOperator from "../binary-operator";

export class BinaryOperation implements ASTNode {
  kind: ASTNodeKind.BinaryOperation;
  children = [null, null];
  hasChildren = () => {
    return this.left() != null && this.right() != null;
  };

  run = () => {
    switch (this.opTok.op) {
      case BinaryOperator.plus:
        return (this.left().run() as number) + (this.right().run() as number);
      case BinaryOperator.minus:
        return (this.left().run() as number) - (this.right().run() as number);
      case BinaryOperator.mult:
        return (this.left().run() as number) * (this.right().run() as number);
      case BinaryOperator.div:
        return (this.left().run() as number) / (this.right().run() as number);
      default:
        throw `Unrecognized BinaryOperator ${this.opTok.op} in BinaryOperation.run()`;
    }
  };

  opTok: BinaryOperatorToken;
  constructor(opTok: BinaryOperatorToken) {
    this.opTok = opTok;
  }

  left(): ASTNode | null {
    return this.children[0];
  }

  right(): ASTNode | null {
    return this.children[1];
  }

  setLeft(node: ASTNode | null) {
    this.children[0] = node;
  }

  setRight(node: ASTNode | null) {
    this.children[1] = node;
  }

  print() {
    console.log(`${this.opTok.op}\t(Result: ${this.run()})`);
  }
}
