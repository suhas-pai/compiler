/*
 * AST/ExprKind.h
 */

#pragma once

namespace AST {
    enum class ExprKind {
        Base,

        BinaryOperation,
        UnaryOperation,
        CharLiteral,
        NumberLiteral,
        FloatLiteral,
        StringLiteral,
        VarDecl,

        Paren,

        FunctionDecl,
        FunctionPrototype,
        Parameter
    };
}