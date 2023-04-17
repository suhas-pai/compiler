import ASTNodeKind from "./Kind";
import { IntegerLiteralToken } from "../token";
import ASTNode from "./Node";

export default class IntegerLiteral implements ASTNode {
  kind: ASTNodeKind.IntegerLiteral;
  children: null;
  hasChildren: () => boolean = () => {
    return false;
  };
  run = () => {
    return this.token.num;
  };

  token: IntegerLiteralToken;
  constructor(token: IntegerLiteralToken) {
    this.token = token;
  }

  print() {
    console.log(this.token.num);
  }
}
