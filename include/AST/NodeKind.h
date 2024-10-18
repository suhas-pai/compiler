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

        DeclRefExpr,
        TypeRef,

        Paren,

        ArrayDecl,
        FunctionDecl,
        LambdaDecl,
        StructDecl,
        EnumDecl,

        LvalueNamedDecl,
        EnumMemberDecl,

        FieldDecl,
        VarDecl,
        ParamVarDecl,

        CallExpr,
        FieldExpr,
        ArraySubscriptExpr,

        IfStmt,
        ReturnStmt,

        CompountStmt,

        LvalueNamedDeclBase = LvalueNamedDecl,
        LvalueNamedDeclLast = ParamVarDecl,

        LvalueTypedDeclBase = FieldDecl,
        LvalueTypedDeclLast = ParamVarDecl
    };
}
