/*
 * include/AST/FunctionDecl.h
 */

#pragma once

#include <string>
#include <vector>

#include "AST/Expr.h"
#include "AST/FunctionProtoype.h"
#include "AST/ParamDecl.h"

namespace AST {
    struct FunctionDecl : public Expr {
    protected:
        FunctionPrototype *Prototype;
        Expr *Body;
    public:
        constexpr explicit
        FunctionDecl(FunctionPrototype *const Protoype,
                     Expr *const Body = nullptr) noexcept
        : Expr(ExprKind::FunctionDecl), Prototype(Protoype), Body(Body) {}

        [[nodiscard]] constexpr auto getPrototype() const noexcept {
            return Prototype;
        }

        [[nodiscard]] constexpr auto getBody() const noexcept {
            return Body;
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

        llvm::Value *codegen(Backend::LLVM::Handler &Handler) noexcept override;
    };
}