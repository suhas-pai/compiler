/*
 * AST/CommaSepCompoundStmt.h
 * © suhas pai
 */

#pragma once

#include <vector>
#include "AST/Stmt.h"

namespace AST {
    class CommaSepStmtList : public Stmt {
    public:
        constexpr static auto ObjKind = NodeKind::CommaSepStmtList;
    protected:
        std::vector<Stmt *> StmtList;
    public:
        constexpr explicit
        CommaSepStmtList(const std::vector<Stmt *> &StmtList) noexcept
        : Stmt(ObjKind), StmtList(StmtList) {}

        constexpr explicit
        CommaSepStmtList(std::vector<Stmt *> &&StmtList) noexcept
        : Stmt(ObjKind), StmtList(std::move(StmtList)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto &getStmtList() const noexcept {
            return StmtList;
        }

        [[nodiscard]] constexpr auto &getStmtListRef() noexcept {
            return StmtList;
        }
    };
}