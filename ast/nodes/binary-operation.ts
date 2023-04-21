import ASTNode, { ASTNodeLink } from "../node";
import ASTNodeKind from "../kind";
import { BinaryOperatorToken } from "../../token";
import { BinaryOperator } from "../../operators";

export default class BinaryOperation implements ASTNode {
  readonly kind = ASTNodeKind.BinaryOperation;
  children = [null, null];
  link: ASTNodeLink = new ASTNodeLink();

  hasChildren = () => {
    return this.left() != null && this.right() != null;
  };

  addChild = (node: ASTNode) => {
    if (this.right() != null) {
      throw "BinaryOperation already has a right node";
    }

    this.setRight(node);
  };

  removeChild = (index: number): ASTNode | undefined => {
    let oldValue: ASTNode | undefined;
    if (index == 0 || index == 1) {
      oldValue = this.children[index];
      this.children[index] = null;
    }

    return oldValue;
  };

  run = () => {
    switch (this.token.op) {
      case BinaryOperator.plus:
        return (this.left().run() as number) + (this.right().run() as number);
      case BinaryOperator.minus:
        return (this.left().run() as number) - (this.right().run() as number);
      case BinaryOperator.mult:
        return (this.left().run() as number) * (this.right().run() as number);
      case BinaryOperator.modulo:
        return (this.left().run() as number) % (this.right().run() as number);
      case BinaryOperator.div:
        return (this.left().run() as number) / (this.right().run() as number);
      case BinaryOperator.power:
        return (this.left().run() as number) ** (this.right().run() as number);
      default:
        throw `Unrecognized BinaryOperator "${this.token.op}" in BinaryOperation.run()`;
    }
  };

  token: BinaryOperatorToken;
  constructor(token: BinaryOperatorToken, left?: ASTNode, right?: ASTNode) {
    this.token = token;
    this.setLeft(left);
    this.setRight(right);
  }

  left(): ASTNode | null {
    return this.children[0];
  }

  right(): ASTNode | null {
    return this.children[1];
  }

  setLeft(node?: ASTNode) {
    node?.link.removeLink();
    node?.link.setToParent(this, 0);
    this.children[0] = node;
  }

  setRight(node?: ASTNode) {
    node?.link.removeLink();
    node?.link.setToParent(this, 1);

    this.children[1] = node;
  }

  print(prefix: string) {
    console.log(`${prefix} ${this.token.op}\t(Result: ${this.run()})`);
  }
}
