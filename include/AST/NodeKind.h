/*
 * AST/NodeKind.h
 */

#pragma once

namespace AST {
    enum class NodeKind {
        Base,

        BinaryOperation,
        UnaryOperation,
        CharLiteral,
        NumberLiteral,
        FloatLiteral,
        StringLiteral,
        VariableRef,

        Paren,

        VarDecl,
        DeclBase = VarDecl,
        FunctionDecl,
        DeclLast = FunctionDecl,

        FunctionPrototype,
        FunctionCall,

        IfStmt,
        ReturnStmt,

        CompountStmt
    };
}