import BinaryOperation from "./ast/binary-operation";
import ASTNode from "./ast/node";

export default class AST {
  root?: ASTNode;
  currentOperation?: BinaryOperation; // Operation at the bottom most right of tree

  getSpaceString(spaces: number): string {
    let space_string = "";
    for (let i = 0; i != spaces; i++) {
      space_string += " ";
    }

    return space_string;
  }

  in(node: ASTNode, spaces: number, tab: number): void {
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

  run(): Object {
    return this.root?.run();
  }
}
