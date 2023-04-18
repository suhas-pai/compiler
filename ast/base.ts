import ASTNode from "./node";

export default class AST {
  root?: ASTNode;
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

  setRoot(root?: ASTNode): void {
    this.root = root;
    if (this.root) {
      this.root.link.removeLink();
      this.root.link.setToTree(this);
    }
  }

  run(): Object {
    return this.root?.run();
  }
}
