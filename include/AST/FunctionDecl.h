/*
 * include/AST/FunctionDecl.h
 */

#pragma once

#include <string>
#include <vector>

#include "AST/FunctionProtoype.h"

namespace AST {
    struct FunctionDecl : public Stmt {
    public:
        constexpr static auto ObjKind = ExprKind::FunctionDecl;
    protected:
        SourceLocation Loc;
        FunctionPrototype *Prototype;
        Expr *Body;
    public:
        constexpr explicit
        FunctionDecl(FunctionPrototype *const Protoype,
                     Expr *const Body = nullptr) noexcept
        : Stmt(ObjKind), Prototype(Protoype), Body(Body) {}

        [[nodiscard]] static inline auto IsOfKind(const Expr &Expr) noexcept {
            return (Expr.getKind() == ObjKind);
        }

        [[nodiscard]]
        static inline auto classof(const Expr *const Obj) noexcept {
            return IsOfKind(*Obj);
        }

        [[nodiscard]] constexpr auto getPrototype() const noexcept {
            return Prototype;
        }

        [[nodiscard]] constexpr auto getBody() const noexcept {
            return Body;
        }

        [[nodiscard]] constexpr auto getLoc() const noexcept {
            return Loc;
        }

        constexpr auto setPrototype(FunctionPrototype *const Protoype) noexcept
            -> decltype(*this)
        {
            this->Prototype = Protoype;
            return *this;
        }

        constexpr auto setBody(Expr *const Body) noexcept -> decltype(*this) {
            this->Body = Body;
            return *this;
        }

        constexpr auto setLoc(SourceLocation Loc) noexcept -> decltype(*this) {
            this->Loc = Loc;
            return *this;
        }

        llvm::Value *codegen(Backend::LLVM::Handler &Handler) noexcept override;
    };
}