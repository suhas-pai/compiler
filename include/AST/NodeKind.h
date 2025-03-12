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
        ParenExpr,

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

        ArrayType,
        ClosureType,
        EnumType,
        FunctionType,
        OptionalType,
        PointerType,
        ShapeType,
        StructType,
        UnionType,

        IfStmt,

        CompoundStmt,
        ForStmt,

        CommaSepStmtList,
        ReturnStmt,

        TypeBase = ArrayType,
        TypeLast = UnionType,

        LvalueNamedDeclBase = LvalueNamedDecl,
        LvalueNamedDeclLast = ParamVarDecl,

        LvalueTypedDeclBase = FieldDecl,
        LvalueTypedDeclLast = ParamVarDecl,

        ExprBase = BinaryOperation,
        ExprLast = UnionType,
    };
}
