import BinaryOperation from "./ast/binary-operation";
import ASTNode from "./ast/Node";

export default class AST {
  root?: ASTNode;
  currentOperation?: BinaryOperation; // Operation at the bottom most right of tree

  private getSpaceString(spaces: number): string {
    let spaceString = "";
    for (let i = 0; i != spaces; i++) {
      spaceString += " ";
    }

    return spaceString;
  }

  private create(node: ASTNode, spaces: number, tab: number): void {
    node.print(this.getSpaceString(spaces));
    if (!node.hasChildren()) {
      return;
    }

    for (const child of node.children) {
      this.create(child, spaces + tab, tab);
    }
  }

  createTree(tab: number = 4): void {
    if (this.root) {
      this.create(this.root, 0, tab);
    }
  }

  run(): Object {
    return this.root?.run();
  }
}
