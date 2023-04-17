import {Operator} from './operation';

export enum TokenKind{
    Number = "number",
    Operator = "operator"
}

const spaces: string[] = [' ', '\n', '\t', '\r', '\f'];
export class TokenNumber {kind: TokenKind.Number; num: Number};
export class TokenOperator {kind: TokenKind.Operator; op: Operator};
export type Token = TokenNumber | TokenOperator;

export class Lexer {
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
                case Object.values(Operator).includes(char as Operator):
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
            let token: Token | null = null
            switch (true) {
                case spaces.includes(char):
                    // spaces
                    // skip for now
                    continue;
                case char >= '0' && char <= '9':
                    // digits
                    let [num, index] = this.readNumber(expr, i)
                    i = index

                    token = new TokenNumber()
                    token.num = num
                    break;
                case Object.values(Operator).includes(char as Operator): // figure something else out for this
                    token = new TokenOperator();
                    token.op = char as Operator;

                    break;
                default: 
                    throw `Not an allowed character ${char}`;
            }

            tokens.push(token as Token)
        }
        return tokens
    }
}

