/*
 * AST/CompoundStmt.h
 */

#pragma once
#include "AST/Stmt.h"
#include "Basic/SourceLocation.h"

namespace AST {
    struct CompoundStmt : public Stmt {
    public:
        constexpr static auto ObjKind = NodeKind::CompountStmt;
    protected:
        SourceLocation BraceLoc;
        std::vector<Stmt *> StmtList;
    public:
        constexpr explicit
        CompoundStmt(const SourceLocation BraceLoc,
                     std::vector<Stmt *> &&StmtList) noexcept
        : Stmt(ObjKind), BraceLoc(BraceLoc), StmtList(StmtList) {}

        [[nodiscard]]
        static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return (Stmt.getKind() == ObjKind);
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
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

        [[nodiscard]] std::optional<llvm::Value *>
        codegen(Backend::LLVM::Handler &Handler,
                llvm::IRBuilder<> &Builder,
                Backend::LLVM::ValueMap &ValueMap) noexcept;
    };
}