/*
 * include/AST/FunctionDecl.h
 */

#pragma once

#include <string_view>

#include "AST/Decl.h"
#include "AST/FunctionProtoype.h"

namespace AST {
    struct FunctionDecl : public Decl {
    public:
        constexpr static auto ObjKind = NodeKind::FunctionDecl;
    protected:
        SourceLocation Loc;
        FunctionPrototype *Prototype;
        Stmt *Body;
        bool IsExternal : 1;
    public:
        constexpr explicit
        FunctionDecl(FunctionPrototype *const Protoype,
                     Stmt *const Body = nullptr,
                     bool IsExternal = false) noexcept
        : Decl(ObjKind), Prototype(Protoype), Body(Body),
          IsExternal(IsExternal) {}

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

        [[nodiscard]] constexpr auto isExternal() const noexcept {
            return IsExternal;
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

        constexpr auto setBody(Stmt *const Body) noexcept -> decltype(*this) {
            this->Body = Body;
            return *this;
        }

        constexpr auto setLoc(const SourceLocation Loc) noexcept
            -> decltype(*this)
        {
            this->Loc = Loc;
            return *this;
        }

        constexpr auto setIsExternal(const bool IsExternal) noexcept
            -> decltype(*this)
        {
            this->IsExternal = IsExternal;
            return *this;
        }

        [[nodiscard]] auto
        finishPrototypeCodegen(
            Backend::LLVM::Handler &Handler,
            llvm::IRBuilder<> &Builder,
            Backend::LLVM::ValueMap &ValueMap,
            llvm::Value *ProtoCodegen) noexcept
                -> std::optional<llvm::Value *>;

        [[nodiscard]] std::optional<llvm::Value *>
        codegen(Backend::LLVM::Handler &Handler,
                llvm::IRBuilder<> &Builder,
                Backend::LLVM::ValueMap &ValueMap) noexcept override;
    };
}