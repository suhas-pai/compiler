/*
 * AST/PointerExpr.h
 * © suhas pai
 */

 #pragma once

 #include "AST/Expr.h"
 #include "Source/SourceLocation.h"

 namespace AST {
     struct OptionalExpr : public Expr {
     public:
         constexpr static auto ObjKind = NodeKind::CallExpr;
     protected:
         Expr *Operand;
         SourceLocation Loc;
     public:
         constexpr explicit
         OptionalExpr(const SourceLocation Loc, Expr *const Operand) noexcept
         : Expr(ObjKind), Operand(Operand), Loc(Loc) {}

         [[nodiscard]]
         constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
             return Stmt.getKind() == ObjKind;
         }

         [[nodiscard]]
         constexpr static auto classof(const Stmt *const Node) noexcept {
             return IsOfKind(*Node);
         }

         [[nodiscard]] constexpr auto getOperand() const noexcept {
             return this->Operand;
         }

         [[nodiscard]] constexpr auto getLoc() const noexcept {
             return this->Loc;
         }

         constexpr auto setOperand(Expr *const Operand) noexcept
             -> decltype(*this)
         {
             this->Operand = Operand;
             return *this;
         }
     };
 }
