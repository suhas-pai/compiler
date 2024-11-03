/*
 * AST/ForStmt.h
 * © suhas pai
 */

#pragma once
#include "Basic/SourceLocation.h"

#include "CommaSepStmtList.h"
#include "Expr.h"

namespace AST {
    class ForStmt : public Expr {
    public:
        constexpr static auto ObjKind = NodeKind::ForStmt;
    protected:
        SourceLocation ForLoc;

        CommaSepStmtList *Init;
        Expr *Cond;
        CommaSepStmtList *Step;

        Stmt *Body;
    public:
        constexpr explicit
        ForStmt(const SourceLocation ForLoc,
                CommaSepStmtList *const Init,
                Expr *const Cond,
                CommaSepStmtList *const Step,
                Stmt *const Body)
        : Expr(ObjKind), ForLoc(ForLoc), Init(Init), Cond(Cond), Step(Step),
          Body(Body) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getForLoc() const noexcept {
            return ForLoc;
        }

        [[nodiscard]] constexpr auto getInit() const noexcept {
            return Init;
        }

        [[nodiscard]] constexpr auto getCond() const noexcept {
            return Cond;
        }

        [[nodiscard]] constexpr auto getIncr() const noexcept {
            return Step;
        }

        [[nodiscard]] constexpr auto getBody() const noexcept {
            return Body;
        }

        constexpr auto setInit(CommaSepStmtList *const Init) noexcept
            -> decltype(*this)
        {
            this->Init = Init;
            return *this;
        }

        constexpr auto setCond(Expr *const Cond) noexcept -> decltype(*this) {
            this->Cond = Cond;
            return *this;
        }

        constexpr auto setStep(CommaSepStmtList *const Step) noexcept
            -> decltype(*this)
        {
            this->Step = Step;
            return *this;
        }

        constexpr auto setBody(Stmt *const Body) noexcept -> decltype(*this) {
            this->Body = Body;
            return *this;
        }
    };
}