/*
 * AST/TypeRef.h
 * © suhas pai
 */

#pragma once

#include <string>
#include <vector>

#include "AST/Expr.h"
#include "Basic/SourceLocation.h"
#include "Sema/Types/Qualifiers.h"

namespace AST {
    struct TypeRef : public Expr {
    public:
        enum InstKind {
            Pointer,
            Type,
        };

        struct Inst {
        private:
            InstKind Kind;
        protected:
            constexpr explicit Inst(const InstKind Kind) noexcept
            : Kind(Kind) {}
        public:
            [[nodiscard]] constexpr auto getKind() const noexcept {
                return Kind;
            }
        };

        struct PointerInst : public Inst {
        public:
            constexpr static auto ObjKind = InstKind::Pointer;
        protected:
            Sema::TypeQualifiers Qual;
            SourceLocation Loc;
        public:
            constexpr explicit
            PointerInst(const Sema::TypeQualifiers Qual) noexcept
            : Inst(ObjKind), Qual(Qual) {}

            [[nodiscard]]
            static inline auto IsOfKind(const Inst &Inst) noexcept {
                return Inst.getKind() == ObjKind;
            }

            [[nodiscard]]
            static inline auto classof(const Inst *const Inst) noexcept {
                return IsOfKind(*Inst);
            }

            [[nodiscard]] constexpr auto getQualifiers() const noexcept {
                return Qual;
            }
        };

        struct TypeInst : public Inst {
        public:
            constexpr static auto ObjKind = InstKind::Type;
        protected:
            std::string Name;
            SourceLocation NameLoc;

            Sema::TypeQualifiers Qual;
        public:
            constexpr explicit
            TypeInst(const std::string_view Name,
                     const SourceLocation NameLoc,
                     const Sema::TypeQualifiers Qual) noexcept
            : Inst(ObjKind), Name(Name), NameLoc(NameLoc), Qual(Qual) {}

            [[nodiscard]]
            static inline auto IsOfKind(const Inst &Inst) noexcept {
                return Inst.getKind() == ObjKind;
            }

            [[nodiscard]]
            static inline auto classof(const Inst *const Inst) noexcept {
                return IsOfKind(*Inst);
            }

            [[nodiscard]]
            constexpr auto getName() const noexcept -> std::string_view {
                return Name;
            }

            [[nodiscard]] constexpr auto getNameLoc() const noexcept {
                return NameLoc;
            }

            [[nodiscard]] constexpr auto getQualifiers() const noexcept {
                return Qual;
            }
        };

        constexpr static auto ObjKind = NodeKind::TypeRef;
    protected:
        std::vector<Inst *> InstList;
    public:
        constexpr explicit
        TypeRef(std::vector<Inst *> &&InstList) noexcept
        : Expr(ObjKind), InstList(std::move(InstList)) {}

        [[nodiscard]] static inline auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto &getInstList() const noexcept {
            return InstList;
        }
    };
}