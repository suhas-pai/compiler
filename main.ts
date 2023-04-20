import AST from "./ast/base";
import Lexer from "./lexer";
import Parser from "./parser";

const exprs: string[] = ["2+3*4/5"];
// const exprs: string[] = ["(2 + 2)"];
for (const expr of exprs) {
  const lexer = new Lexer(expr);
  const tokens = lexer.lex();

  console.log(tokens);

  const parser = new Parser(tokens);
  const ast = new AST();

  parser.parse(ast);
  ast.inOrder();

  console.log(`Final Result: ${ast.run()}`);
}
