import AST from "./ast";
import Lexer from "./lexer";
import Parser from "./parser";

const exprs: string[] = [
  "0xF + 0o40 * 0b1100 - 2 * 4 - 1 * 8",
  "2 + 4 - 8 * 16",
  "2 + 4 * 8 - 16",
  "8 * 3 - 4",
];
// const exprs: string[] = ["(2 + 2)"];
for (const expr of exprs) {
  const lexer = new Lexer(expr);
  const tokens = lexer.lex();

  console.log(tokens);

  const parser = new Parser(tokens);
  const ast = new AST();

  parser.parse(ast);
  ast.createTree();

  console.log(`Final Result: ${ast.run()}`);
}
