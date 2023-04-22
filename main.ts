import AST from "./ast/base";
import Lexer from "./lexer";
import Parser from "./parser";

const exprs: string[] = [
  'let varName = "Hello"',
  "( 2+ 2)",
  "2*3+4",
  "2+3*4/5",
  "let varName = ~3*2**6/2",
];

for (const expr of exprs) {
  const lexer = new Lexer(expr);
  const tokens = lexer.lex();

  console.log(tokens);

  const parser = new Parser(tokens);
  const ast = new AST();

  parser.parse(ast);

  ast.verify();
  ast.inOrder();

  console.log(`Final Result: ${ast.run()}`);
}
