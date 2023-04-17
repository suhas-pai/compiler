import ASTNodeKind from "./kind";
import ASTNode from "./node";
import { IntegerLiteralToken } from "../token";

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
