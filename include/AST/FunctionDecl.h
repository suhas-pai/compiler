/*
 * include/AST/FunctionDecl.h
 */

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "AST/Decl.h"
#include "AST/FunctionProtoype.h"
#include "AST/Expr.h"

namespace AST {
    struct FunctionDecl : public Decl {
    public:
        constexpr static auto ObjKind = NodeKind::FunctionDecl;
    protected:
        SourceLocation Loc;
        FunctionPrototype *Prototype;
        Expr *Body;
    public:
        constexpr explicit
        FunctionDecl(FunctionPrototype *const Protoype,
                     Expr *const Body = nullptr) noexcept
        : Decl(ObjKind), Prototype(Protoype), Body(Body) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return (Stmt.getKind() == ObjKind);
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
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

        [[nodiscard]]
        constexpr std::string_view getName() const noexcept override {
            return getPrototype()->getName();
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

        [[nodiscard]] llvm::Value *
        finishPrototypeCodegen(
            Backend::LLVM::Handler &Handler,
            Backend::LLVM::ValueMap &ValueMap,
            llvm::Value *ProtoCodegen) noexcept;

        [[nodiscard]] llvm::Value *
        codegen(Backend::LLVM::Handler &Handler,
                Backend::LLVM::ValueMap &ValueMap) noexcept override;
    };
}