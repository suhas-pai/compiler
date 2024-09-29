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
        FunctionPrototype *Prototype;
        Stmt *Body;
    public:
        constexpr explicit
        FunctionDecl(FunctionPrototype *const Prototype,
                     Stmt *const Body,
                     const Linkage Linkage) noexcept
        : Decl(ObjKind, Prototype->getNameLoc(), Linkage), Prototype(Prototype),
          Body(Body) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto &getPrototype() const noexcept {
            return *Prototype;
        }

        [[nodiscard]] constexpr auto &getBody() const noexcept {
            return *Body;
        }

        [[nodiscard]]
        constexpr std::string_view getName() const noexcept override {
            return getPrototype().getName();
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
    };
}