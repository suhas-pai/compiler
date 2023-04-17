import ASTNodeKind from "./Kind";
import { IntegerLiteralToken } from "../token";
import ASTNode from "./Node";

export default class IntegerLiteral implements ASTNode {
  kind: ASTNodeKind.IntegerLiteral;
  children: null;
  hasChildren = () => {
    return false;
  };
  run = () => {
    return this.token.literal;
  };

  token: IntegerLiteralToken;
  constructor(token: IntegerLiteralToken) {
    this.token = token;
  }

  print(prefix: string) {
    console.log(`${prefix}${this.token.literal}`);
  }
}
