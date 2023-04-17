import ASTNodeKind from "./kind";

export class ASTNodeLink {
  parent?: ASTNode;
  indexInChildren: number = -1;

  setToParent(parent: ASTNode, indexInChildren: number) {
    this.parent = parent;
    this.indexInChildren = indexInChildren;
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
