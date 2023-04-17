import { assert } from 'console';
import {Token, TokenKind, TokenOperator} from './lexer'

export enum Operator {
    minus = '-',
    plus = '+',
    mult = '*',
    div = '/'
};

export class Node{
    val: Token;
    left: Node | null = null
    right: Node | null = null

    constructor(val: Token) {
        this.val = val
    }

    in() {
        console.log(this)
        if (this.left != null) {
            this.left.in()
        } else {
            console.log("(null)")
        }
        
        if (this.right != null) {
            this.right.in()
        } else {
            console.log("(null)")
        }
    }
}

export class Parser{
    root: Node;
    curOp: Node;

    pop(tokens: Token[], kind: TokenKind): Token {
        let token = tokens.shift()
        assert((token as Token).kind == kind, `Token must be of type ${kind}`);

        return tokens[0]
    }

    parse(tokens: Token[]): Node{
        if (this.root == null){
            let token = this.pop(tokens, TokenKind.Number)
            this.root = new Node(token)
        }

        // Only 1 element in tree (a number)
        if (this.curOp == null) {
            let token = this.pop(tokens, TokenKind.Operator)
            this.curOp = new Node(token)
            this.curOp.left = this.root
            this.root = this.curOp
        }

        while (true) {
            if (this.curOp.right == null) {
                this.curOp.right = new Node(this.pop(tokens, TokenKind.Number))
                continue
            }

            let opToken = this.pop(tokens, TokenKind.Operator) as TokenOperator
            let curOp = (this.curOp.val as TokenOperator).op

            if (Parser.precedence(curOp) <= Parser.precedence(opToken.op)) {
                this.curOp = new Node(opToken)
                this.curOp.left = this.root
                this.root = this.curOp

                continue
            }

            let newOp = new Node(opToken)
            newOp.left = this.curOp.right

            this.curOp.right = newOp
            this.curOp = newOp
        }

        return this.root
    }

    static precedence(operator: Operator): Number{
        switch(operator){
            case Operator.plus:
            case Operator.minus:
                return 0
            case Operator.div:
            case Operator.mult:
                return 1
            default:
                throw "Illegal operator type (How did this happen??)"
        }
    }

    inOrder() {
        this.root.in()
    }
}