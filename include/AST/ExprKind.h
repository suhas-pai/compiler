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
        IntegerLiteral,
        FloatLiteral,
        StringLiteral,
        VarDecl,

        Paren,
    };
}