import ASTNodeKind from "../kind";
import ASTNode, { ASTNodeLink } from "../node";

export default class GlobalScope implements ASTNode {
  kind = ASTNodeKind.ParentExpression;
  children: ASTNode[] = [];
  link: ASTNodeLink = new ASTNodeLink();

  hasChildren = () => {
    return this.children.length != 0;
  };

  addChild = (child: ASTNode) => {
    this.children.push(child);
  };

  removeChild = (index: number): ASTNode | undefined => {
    let oldValue: ASTNode | undefined;
    if (index < this.children.length) {
      oldValue = this.children[index];
      this.children.splice(index);
    }

    return oldValue;
  };

  print = (prefix: string) => {
    console.log(`${prefix}Global-Scope`);
  };
  run = () => {
    let result = 0;
    for (let child of this.children) {
      result += child.run() as number;
    }

    return result;
  };
}
