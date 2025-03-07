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
        Paren,

        ArrayDecl,
        ClosureDecl,
        EnumDecl,
        FunctionDecl,
        StructDecl,

        LvalueNamedDecl,
        EnumMemberDecl,

        FieldDecl,
        VarDecl,
        ParamVarDecl,

        ArrayDestructuredVarDecl,
        ObjectDestructuredVarDecl,

        CallExpr,
        FieldExpr,
        ArraySubscriptExpr,
        CastExpr,

        IfStmt,

        CompoundStmt,
        ForStmt,

        CommaSepStmtList,
        ReturnStmt,

        ArrayType,
        ClosureType,
        EnumType,
        FunctionType,
        OptionalType,
        PointerType,
        ShapeType,
        StructType,
        UnionType,

        TypeBase = ArrayType,
        TypeLast = UnionType,

        LvalueNamedDeclBase = LvalueNamedDecl,
        LvalueNamedDeclLast = ParamVarDecl,

        LvalueTypedDeclBase = FieldDecl,
        LvalueTypedDeclLast = ParamVarDecl,

        DeclStmtBase = ArrayDecl,
        DeclStmtLast = ObjectDestructuredVarDecl,

        ExprBase = BinaryOperation,
        ExprLast = ForStmt,

    };
}
