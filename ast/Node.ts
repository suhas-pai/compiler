import AST from "./base";
import ASTNodeKind from "./kind";

export class ASTNodeLink {
  parent?: ASTNode;
  indexInChildren: number = -1;
  tree?: AST;

  setToParent(parent: ASTNode, indexInChildren: number) {
    this.parent = parent;
    this.indexInChildren = indexInChildren;
  }

  setToTree(tree: AST) {
    this.tree = tree;
  }
}

export default interface ASTNode {
  kind: ASTNodeKind;
  children: ASTNode[];
  link: ASTNodeLink;
  hasChildren: () => boolean;
  print: (prefix: string) => void;
  run: () => Object;
}
