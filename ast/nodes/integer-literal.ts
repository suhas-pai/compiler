import ASTNodeKind from "../kind";
import ASTNode, { ASTNodeLink } from "../node";
import { IntegerLiteralToken } from "../../token";

export default class IntegerLiteral implements ASTNode {
  readonly kind = ASTNodeKind.IntegerLiteral;
  children: null;
  link: ASTNodeLink = new ASTNodeLink();

  hasChildren = () => {
    return false;
  };

  addChild = () => {
    throw "Can't add child to IntegerLiteral";
  };

  removeChild = (): ASTNode | undefined => {
    return undefined;
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
