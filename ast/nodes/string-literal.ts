import ASTNodeKind from "../kind";
import ASTNode, { ASTNodeLink } from "../node";
import { StringLiteralToken } from "../../token";

export default class StringLiteral implements ASTNode {
  readonly kind = ASTNodeKind.StringLiteral;
  children: null;
  link: ASTNodeLink = new ASTNodeLink();

  hasChildren = () => {
    return false;
  };

  addChild = () => {
    throw "Can't add child to StringLiteral";
  };

  removeChild = (): ASTNode | undefined => {
    return undefined;
  };

  run = () => {
    return this.token.literal;
  };

  token: StringLiteralToken;
  constructor(token: StringLiteralToken) {
    this.token = token;
  }

  print(prefix: string) {
    console.log(`${prefix}${this.token.literal}`);
  }
}
