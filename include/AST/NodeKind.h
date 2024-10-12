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

        VarDecl,
        ParamVarDecl,
        FunctionDecl,
        FieldDecl,
        StructDecl,

        EnumMemberDecl,
        EnumDecl,

        CallExpr,
        FieldExpr,
        ArraySubscriptExpr,

        IfStmt,
        ReturnStmt,

        CompountStmt,

        DeclBase = VarDecl,
        DeclLast = EnumDecl,

        NamedDeclBase = VarDecl,
        NamedDeclLast = EnumDecl,
    };
}