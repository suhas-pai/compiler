/*
 * AST/Decls/LambdaDecl.h
 * © suhas pai
 */

#pragma once

#include "ArrayDecl.h"
#include "FunctionDecl.h"

namespace AST {
    class LambdaDecl : public FunctionDecl {
    public:
        constexpr static auto ObjKind = NodeKind::LambdaDecl;
    protected:
        ArrayDecl *CaptureList;
    public:
        constexpr explicit
        LambdaDecl(const SourceLocation Loc,
                   ArrayDecl *const CaptureList,
                   const std::vector<ParamVarDecl *> &ParamList,
                   Sema::Type *const ReturnType,
                   Stmt *const Body,
                   const enum Linkage Linkage) noexcept
        : FunctionDecl(Loc, ParamList, ReturnType, Body, Linkage),
          CaptureList(CaptureList) {}

        constexpr explicit
        LambdaDecl(const SourceLocation Loc,
                   ArrayDecl *const CaptureList,
                   std::vector<ParamVarDecl *> &&ParamList,
                   Sema::Type *const ReturnType,
                   Stmt *const Body,
                   const enum Linkage Linkage) noexcept
        : FunctionDecl(Loc, std::move(ParamList), ReturnType, Body, Linkage),
          CaptureList(CaptureList) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getCaptureList() const noexcept {
            return CaptureList;
        }
    };
}
