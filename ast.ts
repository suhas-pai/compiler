import BinaryOperation from "./ast/BinaryOperation";
import ASTNode from "./ast/Node";

export class AST {
  root?: ASTNode;
  currentOperation?: BinaryOperation; // Operation at the bottom most right of tree

  writeSpaces(spaces: number) {
    for (let i = 0; i != spaces; i++) {
      process.stdout.write(" ");
    }
  }

  in(node: ASTNode, spaces: number, tab: number) {
    this.writeSpaces(spaces);
    node.print();

    if (!node.hasChildren()) {
      return;
    }

    for (const child of node.children) {
      this.in(child, spaces + tab, tab);
    }
  }

  inOrder(tab: number = 4) {
    if (this.root) {
      this.in(this.root, 0, tab);
    }
  }

  run(): Object {
    return this.root?.run();
  }
}
