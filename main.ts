import { type } from "os";
import { Lexer } from './lexer'
import { Parser } from './parser'

let expr = "2 + 4 - 8"
let tokens = Lexer.lex(expr)

console.log(tokens)

let parser = new Parser()
let ast = parser.parse(tokens)

console.log(ast.in())
