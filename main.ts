import { type } from "os";

const spaces: string[] = [' ', '\n', '\t', '\r', '\f'];
enum operator {
    minus = '-',
    plus = '+',
    mult = '*',
    div = '/'
};

type Token = number | operator;

class digit {
    val: number = 0
}

class Lexer {
    static readNumber(expr: string, i: number): [number, number]{
        let number = 0;
        for (; i < expr.length; i++){
            let char = expr.charAt(i);
            switch(true){
                case char >= '0' && char <= '9':
                    number *= 10;
                    number += char.charCodeAt(0) - '0'.charCodeAt(0) // convert char to n
                    break;
                case spaces.includes(char):
                case Object.values(operator).includes(char as operator):
                    return [number, i];
                default:
                    throw "Bad bad character!";

            }
        }
        return [number, i];
    }

    static lex(expr: string): Token[] {
        let tokens: Token[] = []
        for (let i = 0; i < expr.length; i++) {
            let char = expr.charAt(i);
            switch (true) {
                case spaces.includes(char):
                    // spaces
                    // skip for now
                    break;
                case char >= '0' && char <= '9':
                    // digits
                    let [num, index] = this.readNumber(expr, i)
                    i = index
                    tokens.push(num)
                    break;
                case Object.values(operator).includes(char as operator):
                    tokens.push(char as operator)
                    break;
                default: 
                    throw `Not an allowed character ${char}`;
            }
        }
        return tokens
    }
}


let expr = "2 + 4 - 8";
console.log(" " === spaces[0]);
type Stuff = keyof typeof operator;
console.log(Lexer.lex(expr));
