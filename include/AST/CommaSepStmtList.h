/*
 * AST/CommaSepCompoundStmt.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "Stmt.h"

namespace AST {
    class CommaSepStmtList : public Stmt {
    public:
        constexpr static auto ObjKind = NodeKind::CommaSepStmtList;
    protected:
        std::vector<Stmt *> StmtList;
    public:
        constexpr explicit
        CommaSepStmtList(const std::span<Stmt *> StmtList) noexcept
        : Stmt(ObjKind), StmtList(StmtList.begin(), StmtList.end()) {}

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

        [[nodiscard]] constexpr auto getStmtList() const noexcept {
            return std::span(this->StmtList);
        }

        [[nodiscard]] constexpr auto &getStmtListRef() noexcept {
            return this->StmtList;
        }
    };
}