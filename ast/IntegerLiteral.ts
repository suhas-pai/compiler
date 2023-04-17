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
    return this.numTok.num;
  };

  numTok: IntegerLiteralToken;
  constructor(numTok: IntegerLiteralToken) {
    this.numTok = numTok;
  }

  print() {
    console.log(this.numTok.num);
  }
}
