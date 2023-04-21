enum ASTNodeKind {
  GlobalScope = "global-scope",
  IntegerLiteral = "integer-literal",
  StringLiteral = "string-literal",
  BinaryOperation = "binary-operation",
  UnaryOperator = "unary-operator",
  ParentExpression = "paren-expression",
  VariableDecl = "variable-decl",
}

export default ASTNodeKind;
