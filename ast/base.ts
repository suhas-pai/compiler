import BinaryOperation from "./binary-operation";
import ASTNode from "./node";

export default class AST {
  root?: ASTNode;
  currentOperation?: BinaryOperation;
  private getSpaceString(spaces: number): string {
    let spaceString = "";
    for (let i = 0; i != spaces; i++) {
      spaceString += " ";
    }

    return spaceString;
  }

  private in(node: ASTNode, spaces: number, tab: number): void {
    node.print(this.getSpaceString(spaces));
    if (!node.hasChildren()) {
      return;
    }

    for (const child of node.children) {
      this.in(child, spaces + tab, tab);
    }
  }

  inOrder(tab: number = 4): void {
    if (this.root) {
      this.in(this.root, 0, tab);
    }
  }

  push(node: ASTNode) {
    if (this.root != null) {
      throw "Internal Error: Root is already set";
    }

    this.setRoot(node);
  }

  setRoot(root: ASTNode): void {
    this.root = root;
    this.root.link.setToTree(this);
  }

  run(): Object {
    return this.root?.run();
  }
}
