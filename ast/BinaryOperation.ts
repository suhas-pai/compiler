import ASTNode from "./Node";
import ASTNodeKind from "./Kind";
import { BinaryOperatorToken } from "../token";

export class BinaryOperation implements ASTNode {
  kind: ASTNodeKind.BinaryOperation;
  children = [null, null];
  hasChildren = () => {
    return this.left() != null && this.right() != null;
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
    console.log(this.opTok.op);
  }
}
