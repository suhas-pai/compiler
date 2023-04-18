import AST from "./base";
import ASTNodeKind from "./kind";

export class ASTNodeLink {
  parent?: ASTNode;
  indexInChildren: number = -1;
  tree?: AST;

  static fromTree(tree: AST): ASTNodeLink {
    const result = new ASTNodeLink();
    result.setToTree(tree);

    return result;
  }

  static fromParent(parent: ASTNode, index: number): ASTNodeLink {
    const result = new ASTNodeLink();
    result.setToParent(parent, index);

    return result;
  }

  update(node?: ASTNode): ASTNode | undefined {
    let oldValue: ASTNode | undefined;
    if (this.parent) {
      oldValue = this.parent.children[this.indexInChildren];
      this.parent.children[this.indexInChildren] = node;
    } else if (this.tree) {
      oldValue = this.tree.root;
      this.tree.root = node;
    }

    return oldValue;
  }

  setToParent(parent: ASTNode, indexInChildren: number) {
    this.parent = parent;
    this.indexInChildren = indexInChildren;
    this.tree = undefined;
  }

  setToTree(tree: AST) {
    this.tree = tree;

    this.parent = undefined;
    this.indexInChildren = -1;
  }

  removeLink() {
    if (this.parent) {
      this.parent.removeChild(this.indexInChildren);
    } else if (this.tree) {
      this.tree.setRoot(null);
    }
  }
}

export default interface ASTNode {
  readonly kind: ASTNodeKind;
  children: ASTNode[];
  link: ASTNodeLink;
  hasChildren: () => boolean;
  addChild: (node: ASTNode) => void;
  removeChild: (index: number) => ASTNode | undefined;
  print: (prefix: string) => void;
  run: () => Object;
}
