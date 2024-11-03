/*
 * AST/CompoundStmt.h
 */

#pragma once
#include <vector>

#include "AST/Stmt.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct CompoundStmt : public Stmt {
    public:
        constexpr static auto ObjKind = NodeKind::CompoundStmt;
    protected:
        SourceLocation BraceLoc;
        std::vector<Stmt *> StmtList;
    public:
        constexpr explicit
        CompoundStmt(const SourceLocation BraceLoc,
                     const std::vector<Stmt *> &StmtList) noexcept
        : Stmt(ObjKind), BraceLoc(BraceLoc), StmtList(StmtList) {}

        constexpr explicit
        CompoundStmt(const SourceLocation BraceLoc,
                     std::vector<Stmt *> &&StmtList) noexcept
        : Stmt(ObjKind), BraceLoc(BraceLoc), StmtList(std::move(StmtList)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getBraceLoc() const noexcept {
            return BraceLoc;
        }

        [[nodiscard]] constexpr auto &getStmtList() const noexcept {
            return StmtList;
        }

        [[nodiscard]] constexpr auto &getStmtListRef() noexcept {
            return StmtList;
        }

        constexpr auto setBraceLoc(const SourceLocation BraceLoc) noexcept
            -> decltype(*this)
        {
            this->BraceLoc = BraceLoc;
            return *this;
        }
    };
}