import ASTNode from "./Node";
import ASTNodeKind from "./Kind";
import { BinaryOperatorToken } from "../token";
import BinaryOperator from "../binary-operator";

export default class BinaryOperation implements ASTNode {
  kind: ASTNodeKind.BinaryOperation;
  children = [null, null];
  hasChildren = () => {
    return this.left() != null && this.right() != null;
  };

  run = () => {
    switch (this.token.op) {
      case BinaryOperator.plus:
        return (this.left().run() as number) + (this.right().run() as number);
      case BinaryOperator.minus:
        return (this.left().run() as number) - (this.right().run() as number);
      case BinaryOperator.mult:
        return (this.left().run() as number) * (this.right().run() as number);
      case BinaryOperator.div:
        return (this.left().run() as number) / (this.right().run() as number);
      default:
        throw `Unrecognized BinaryOperator "${this.token.op}" in BinaryOperation.run()`;
    }
  };

  token: BinaryOperatorToken;
  constructor(token: BinaryOperatorToken) {
    this.token = token;
  }

  left(): ASTNode | null {
    return this.children[0];
  }

  right(): ASTNode | null {
    return this.children[1];
  }

  setLeft(node?: ASTNode) {
    this.children[0] = node;
  }

  setRight(node?: ASTNode) {
    this.children[1] = node;
  }

  print(prefix: string) {
    console.log(`${prefix} ${this.token.op}\t(Result: ${this.run()})`);
  }
}
