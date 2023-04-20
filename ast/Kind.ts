import { UnaryOperator } from "./../operators";
enum ASTNodeKind {
  GlobalScope = "global-scope",
  IntegerLiteral = "integer-literal",
  BinaryOperation = "binary-operation",
  UnaryOperator = "unary-operator",
  ParentExpression = "paren-expression",
}

export default ASTNodeKind;
