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
        enum class InstKind {
            Pointer,
            Type,
            Array,
            Union,
            Intersect
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
            SourceLocation Loc;
            Sema::TypeQualifiers Qual;
        public:
            constexpr explicit
            PointerInst(const SourceLocation Loc,
                        const Sema::TypeQualifiers Qual) noexcept
            : Inst(ObjKind), Loc(Loc), Qual(Qual) {}

            [[nodiscard]]
            constexpr static auto IsOfKind(const Inst &Inst) noexcept {
                return Inst.getKind() == ObjKind;
            }

            [[nodiscard]]
            constexpr static auto classof(const Inst *const Inst) noexcept {
                return IsOfKind(*Inst);
            }

            [[nodiscard]] constexpr auto getQualifiers() const noexcept {
                return Qual;
            }

            [[nodiscard]] constexpr auto getLoc() const noexcept {
                return Loc;
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
            constexpr static auto IsOfKind(const Inst &Inst) noexcept {
                return Inst.getKind() == ObjKind;
            }

            [[nodiscard]]
            constexpr static auto classof(const Inst *const Inst) noexcept {
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

        struct ArrayInst : public Inst {
        public:
            constexpr static auto ObjKind = InstKind::Array;
        protected:
            SourceLocation Loc;
        public:
            constexpr explicit ArrayInst(const SourceLocation Loc) noexcept
            : Inst(ObjKind), Loc(Loc) {}

            [[nodiscard]]
            constexpr static auto IsOfKind(const Inst &Inst) noexcept {
                return Inst.getKind() == ObjKind;
            }

            [[nodiscard]]
            constexpr static auto classof(const Inst *const Inst) noexcept {
                return IsOfKind(*Inst);
            }

            [[nodiscard]] constexpr auto getLoc() const noexcept {
                return Loc;
            }
        };

        struct UnionInst : public Inst {
        public:
            constexpr static auto ObjKind = InstKind::Type;
        protected:
            SourceLocation Loc;
        public:
            constexpr explicit UnionInst(const SourceLocation NameLoc) noexcept
            : Inst(ObjKind), Loc(NameLoc) {}

            [[nodiscard]]
            constexpr static auto IsOfKind(const Inst &Inst) noexcept {
                return Inst.getKind() == ObjKind;
            }

            [[nodiscard]]
            constexpr static auto classof(const Inst *const Inst) noexcept {
                return IsOfKind(*Inst);
            }

            [[nodiscard]] constexpr auto getLoc() const noexcept {
                return Loc;
            }
        };

        struct IntersectInst : public Inst {
        public:
            constexpr static auto ObjKind = InstKind::Type;
        protected:
            SourceLocation Loc;
        public:
            constexpr explicit IntersectInst(const SourceLocation Loc) noexcept
            : Inst(ObjKind), Loc(Loc) {}

            [[nodiscard]]
            constexpr static auto IsOfKind(const Inst &Inst) noexcept {
                return Inst.getKind() == ObjKind;
            }

            [[nodiscard]]
            constexpr static auto classof(const Inst *const Inst) noexcept {
                return IsOfKind(*Inst);
            }

            [[nodiscard]] constexpr auto getLoc() const noexcept {
                return Loc;
            }
        };

        constexpr static auto ObjKind = NodeKind::TypeRef;
    protected:
        std::vector<Inst *> InstList;
    public:
        constexpr explicit TypeRef(const std::vector<Inst *> &InstList) noexcept
        : Expr(ObjKind), InstList(InstList) {}

        constexpr explicit TypeRef(std::vector<Inst *> &&InstList) noexcept
        : Expr(ObjKind), InstList(std::move(InstList)) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto &getInstList() const noexcept {
            return InstList;
        }
    };
}