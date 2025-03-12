/*
 * AST/Decls/ClosureDecl.h
 * © suhas pai
 */

#pragma once
#include "FunctionDecl.h"

namespace AST {
    class ClosureDecl : public FunctionDecl {
    public:
        constexpr static auto ObjKind = NodeKind::ClosureDecl;
    protected:
        std::vector<Stmt *> CaptureList;
    public:
        explicit
        ClosureDecl(const SourceLocation Loc,
                    const std::span<Stmt *> CaptureList,
                    const std::span<ParamVarDecl *> ParamList,
                    Expr *const ReturnType,
                    Stmt *const Body) noexcept
        : FunctionDecl(Loc, ParamList, ReturnType, Body),
          CaptureList(CaptureList.begin(), CaptureList.end()) {}

        explicit
        ClosureDecl(const SourceLocation Loc,
                    std::vector<Stmt *> &&CaptureList,
                    const std::span<ParamVarDecl *> ParamList,
                    Expr *const ReturnType,
                    Stmt *const Body) noexcept
        : FunctionDecl(Loc, ParamList, ReturnType, Body),
          CaptureList(std::move(CaptureList)) {}

        explicit
        ClosureDecl(const SourceLocation Loc,
                    const std::span<Stmt *> CaptureList,
                    std::vector<ParamVarDecl *> &&ParamList,
                    Expr *const ReturnType,
                    Stmt *const Body) noexcept
        : FunctionDecl(Loc, std::move(ParamList), ReturnType, Body),
          CaptureList(CaptureList.begin(), CaptureList.end()) {}

        explicit
        ClosureDecl(const SourceLocation Loc,
                    std::vector<Stmt *> &&CaptureList,
                    std::vector<ParamVarDecl *> &&ParamList,
                    Expr *const ReturnType,
                    Stmt *const Body) noexcept
        : FunctionDecl(Loc, std::move(ParamList), ReturnType, Body),
          CaptureList(std::move(CaptureList)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getCaptureList() const noexcept {
            return std::span(this->CaptureList);
        }

        [[nodiscard]] constexpr auto &getCaptureListRef() noexcept {
            return this->CaptureList;
        }
    };
}
