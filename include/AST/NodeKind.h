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
        DotIdentifierExpr,
        OptionalUnwrapExpr,
        ParenExpr,

        ArrayDecl,
        ClosureDecl,
        EnumDecl,
        FunctionDecl,
        InterfaceDecl,
        StructDecl,
        ShapeDecl,
        UnionDecl,

        LvalueNamedDecl,
        EnumMemberDecl,

        FieldDecl,
        OptionalFieldDecl,
        VarDecl,
        ParamVarDecl,

        ArrayBindingVarDecl,
        ArrayBindingParamVarDecl,

        ObjectBindingVarDecl,
        ObjectBindingParamVarDecl,

        InlineArrayParamVarDecl,

        CallExpr,
        FieldExpr,
        IfExpr,
        ArraySubscriptExpr,
        CastExpr,
        DerefExpr,

        CaptureAllByRefExpr,
        CaptureAllByValueExpr,

        ArrayType,
        ClosureType,
        EnumType,
        FunctionType,
        OptionalType,
        PointerType,
        ArrayPointerType,
        ShapeType,
        StructType,
        UnionType,

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
