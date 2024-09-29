/*
 * AST/NodeKind.h
 */

#pragma once
#include <cstdint>

namespace AST {
    enum class NodeKind : uint8_t {
        BinaryOperation,
        UnaryOperation,
        CharLiteral,
        NumberLiteral,
        FloatLiteral,
        StringLiteral,
        VariableRef,
        TypeRef, // Forward-decl of type

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