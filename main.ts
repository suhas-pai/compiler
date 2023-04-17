import AST from "./ast";
import Lexer from "./lexer";
import Parser from "./parser";

const exprs: string[] = ["2 + 4 - 8", "2 + 4 - 8 * 16", "2 + 4 * 8 - 16"];
for (const expr of exprs) {
  const tokens = Lexer.lex(expr);
  console.log(tokens);

  const parser = new Parser(tokens);
  const ast = new AST();

  parser.parse(ast);
  ast.inOrder();

  console.log(`Final Result: ${ast.run()}`);
}
