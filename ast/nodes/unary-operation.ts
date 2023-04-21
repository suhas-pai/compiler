import { UnaryOperator } from "../../operators";
import { UnaryOperatorToken } from "../../token";

import ASTNodeKind from "../kind";
import ASTNode, { ASTNodeLink } from "../node";

export default class UnaryOperation implements ASTNode {
  readonly kind = ASTNodeKind.UnaryOperator;
  children = [null];
  link: ASTNodeLink = new ASTNodeLink();

  hasChildren = () => {
    return this.child() != null;
  };

  addChild = (node: ASTNode) => {
    if (this.child() != null) {
      throw "UnaryOperation already has a child";
    }

    this.children[0] = node;
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
      case UnaryOperator.BoolNot:
        return !(this.child().run() as number);
      case UnaryOperator.Invert:
        return ~(this.child().run() as number);
      default:
        throw `Unrecognized UnaryOperator "${this.token.op}" in UnaryOperation.run()`;
    }
  };

  token: UnaryOperatorToken;
  constructor(token: UnaryOperatorToken, child?: ASTNode) {
    this.token = token;
    this.setChild(child);
  }

  child(): ASTNode | null {
    return this.children[0];
  }

  setChild(node?: ASTNode) {
    node?.link.removeLink();
    node?.link.setToParent(this, 0);

    this.children[0] = node;
  }

  print(prefix: string) {
    console.log(`${prefix} ${this.token.op}\t(Result: ${this.run()})`);
  }
}
