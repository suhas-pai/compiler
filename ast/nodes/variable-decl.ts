import ASTNode, { ASTNodeLink } from "../node";
import ASTNodeKind from "../kind";
import { IdentifierToken } from "../../token";

export default class VariableDecl implements ASTNode {
  readonly kind = ASTNodeKind.VariableDecl;
  children = [null];
  link: ASTNodeLink = new ASTNodeLink();

  hasChildren = () => {
    return this.expr() != null;
  };

  addChild = (expr: ASTNode) => {
    if (this.expr() != null) {
      throw "VariableDecl already has an expression";
    }

    this.setExpr(expr);
  };

  removeChild = (): ASTNode | null => {
    let oldValue = this.children[0];
    this.children[0] = null;

    return oldValue;
  };

  expr = (): ASTNode | null => {
    return this.children[0];
  };

  setExpr = (node: ASTNode) => {
    node?.link.removeLink();
    node?.link.setToParent(this, 0);

    this.children[0] = node;
  };

  run = () => {
    return this.expr()?.run();
  };

  nameToken: IdentifierToken;
  constructor(name: IdentifierToken) {
    this.nameToken = name;
  }

  print(prefix: string) {
    console.log(`${prefix}VarDecl\t("${this.nameToken.name}")`);
  }
}
