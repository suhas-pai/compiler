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
        ArrowFunctionDecl,
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

        PointerExpr,
        OptionalExpr,

        IfStmt,

        CompoundStmt,
        ForStmt,

        CommaSepStmtList,
        ReturnStmt,

        LvalueNamedDeclBase = LvalueNamedDecl,
        LvalueNamedDeclLast = ParamVarDecl,

        LvalueTypedDeclBase = FieldDecl,
        LvalueTypedDeclLast = ParamVarDecl,

        DeclStmtBase = ArrayDecl,
        DeclStmtLast = ObjectDestructuredVarDecl,

        ExprBase = BinaryOperation,
        ExprLast = ForStmt,

        ArrayType,
        OptionalType,
        StructType,
        EnumType,
        FunctionType,
        LambdaType,
        PointerType,
        ShapeType,
        UnionType,

        TypeBase = ArrayType,
        TypeLast = UnionType,
    };
}
