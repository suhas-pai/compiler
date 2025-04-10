/*
 * AST/Decls/ObjectDestructureVarDecl.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Decls/ArrayDestructredVarDecl.h"

namespace AST {
    enum class ObjectDestructureFieldKind {
        Identifier,
        Array,
        Object,
        Spread,
    };

    struct ObjectDestructureField {
    protected:
        ObjectDestructureFieldKind Kind;
    public:
        [[nodiscard]] constexpr auto getKind() const noexcept {
            return this->Kind;
        }

        std::string_view Key;
        SourceLocation KeyLoc;

        constexpr explicit
        ObjectDestructureField(const ObjectDestructureFieldKind Kind,
                               const std::string_view Key,
                               const SourceLocation KeyLoc) noexcept
        : Kind(Kind), Key(Key), KeyLoc(KeyLoc) {}
    };

    struct ObjectDestructureFieldIdentifier : public ObjectDestructureField {
        Qualifiers Qualifiers;

        std::string_view Name;
        SourceLocation NameLoc;

        explicit
        ObjectDestructureFieldIdentifier(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const std::string_view Name,
            const SourceLocation NameLoc,
            const struct Qualifiers &Qualifiers) noexcept
        : ObjectDestructureField(ObjectDestructureFieldKind::Identifier, Key,
                                 KeyLoc),
          Qualifiers(Qualifiers), Name(Name), NameLoc(NameLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ObjectDestructureField &Field) noexcept {
            return Field.getKind() == ObjectDestructureFieldKind::Identifier;
        }

        [[nodiscard]] constexpr static
        auto classof(const ObjectDestructureField *const Field) noexcept {
            return IsOfKind(*Field);
        }
    };

    struct ArrayDestructureItem;
    struct ObjectDestructureFieldArray : public ObjectDestructureField {
        Qualifiers Qualifiers;
        std::vector<ArrayDestructureItem *> ItemList;

        explicit
        ObjectDestructureFieldArray(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const struct Qualifiers &Qualifiers,
            const std::span<ArrayDestructureItem *> ItemList) noexcept
        : ObjectDestructureField(ObjectDestructureFieldKind::Array, Key,
                                 KeyLoc),
          Qualifiers(Qualifiers), ItemList(ItemList.begin(), ItemList.end()) {}

        explicit
        ObjectDestructureFieldArray(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const struct Qualifiers &Qualifiers,
            std::vector<ArrayDestructureItem *> &&ItemList) noexcept
        : ObjectDestructureField(ObjectDestructureFieldKind::Array, Key,
                                 KeyLoc),
          Qualifiers(Qualifiers), ItemList(std::move(ItemList)) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ObjectDestructureField &Field) noexcept {
            return Field.getKind() == ObjectDestructureFieldKind::Array;
        }

        [[nodiscard]] constexpr static
        auto classof(const ObjectDestructureField *const Field) noexcept {
            return IsOfKind(*Field);
        }
    };

    struct ObjectDestructureFieldObject : public ObjectDestructureField {
        Qualifiers Qualifiers;
        std::vector<ObjectDestructureField *> FieldList;

        explicit
        ObjectDestructureFieldObject(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const struct Qualifiers &Qualifiers,
            const std::span<ObjectDestructureField *> FieldList) noexcept
        : ObjectDestructureField(ObjectDestructureFieldKind::Object, Key,
                                 KeyLoc),
          Qualifiers(Qualifiers),
          FieldList(FieldList.begin(), FieldList.end()) {}

        explicit
        ObjectDestructureFieldObject(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const struct Qualifiers &Qualifiers,
            std::vector<ObjectDestructureField *> &&FieldList) noexcept
        : ObjectDestructureField(ObjectDestructureFieldKind::Object, Key,
                                 KeyLoc),
          Qualifiers(Qualifiers), FieldList(std::move(FieldList)) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ObjectDestructureField &Field) noexcept {
            return Field.getKind() == ObjectDestructureFieldKind::Object;
        }

        [[nodiscard]] constexpr static
        auto classof(const ObjectDestructureField *const Field) noexcept {
            return IsOfKind(*Field);
        }
    };

    struct ObjectDestructureFieldSpread : public ObjectDestructureField {
        Qualifiers Qualifiers;
        SourceLocation SpreadLoc;

        explicit
        ObjectDestructureFieldSpread(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const SourceLocation SpreadLoc,
            const struct Qualifiers &Qualifiers) noexcept
        : ObjectDestructureField(ObjectDestructureFieldKind::Spread, Key,
                                 KeyLoc),
          Qualifiers(Qualifiers), SpreadLoc(SpreadLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ObjectDestructureField &Field) noexcept {
            return Field.getKind() == ObjectDestructureFieldKind::Spread;
        }

        [[nodiscard]] constexpr static
        auto classof(const ObjectDestructureField *const Field) noexcept {
            return IsOfKind(*Field);
        }
    };

    struct ObjectDestructuredVarDecl : public Stmt {
    protected:
        Qualifiers Qualifiers;
        std::vector<ObjectDestructureField *> FieldList;

        Expr *InitExpr;
    public:
        explicit
        ObjectDestructuredVarDecl(
            const SourceLocation Loc,
            const struct Qualifiers &Qualifiers,
            const std::span<ObjectDestructureField *> FieldList,
            Expr *const InitExpr) noexcept
        : Stmt(NodeKind::ObjectDestructuredVarDecl), Qualifiers(Qualifiers),
          FieldList(FieldList.begin(), FieldList.end()), InitExpr(InitExpr) {}

        explicit
        ObjectDestructuredVarDecl(
            const SourceLocation Loc,
            const struct Qualifiers &Qualifiers,
            std::vector<ObjectDestructureField *> &&FieldList,
            Expr *const InitExpr) noexcept
        : Stmt(NodeKind::ObjectDestructuredVarDecl), Qualifiers(Qualifiers),
          FieldList(std::move(FieldList)), InitExpr(InitExpr) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == NodeKind::ObjectDestructuredVarDecl;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Stmt) noexcept {
            return IsOfKind(*Stmt);
        }

        [[nodiscard]] constexpr auto getFieldList() const noexcept {
            return std::span(this->FieldList);
        }

        [[nodiscard]] constexpr auto &getFieldListRef() noexcept {
            return this->FieldList;
        }

        [[nodiscard]] constexpr auto &getQualifiers() const noexcept {
            return this->Qualifiers;
        }

        [[nodiscard]] constexpr auto &getQualifiers() noexcept {
            return this->Qualifiers;
        }

        [[nodiscard]] constexpr auto getInitExpr() const noexcept {
            return this->InitExpr;
        }
    };
}
