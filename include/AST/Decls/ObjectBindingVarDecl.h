/*
 * AST/Decls/ObjectBindingVarDecl.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "AST/Decls/ArrayBindingVarDecl.h"

namespace AST {
    enum class ObjectBindingFieldKind {
        Identifier,
        Array,
        Object,
        Spread,
    };

    struct ObjectBindingField {
    protected:
        ObjectBindingFieldKind Kind;
    public:
        [[nodiscard]] constexpr auto getKind() const noexcept {
            return this->Kind;
        }

        std::string_view Key;
        SourceLocation KeyLoc;

        constexpr explicit
        ObjectBindingField(const ObjectBindingFieldKind Kind,
                           const std::string_view Key,
                           const SourceLocation KeyLoc) noexcept
        : Kind(Kind), Key(Key), KeyLoc(KeyLoc) {}
    };

    struct ObjectBindingFieldIdentifier : public ObjectBindingField {
        Qualifiers Qualifiers;

        std::string_view Name;
        SourceLocation NameLoc;

        explicit
        ObjectBindingFieldIdentifier(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const std::string_view Name,
            const SourceLocation NameLoc,
            const struct Qualifiers &Qualifiers) noexcept
        : ObjectBindingField(ObjectBindingFieldKind::Identifier, Key, KeyLoc),
          Qualifiers(Qualifiers), Name(Name), NameLoc(NameLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ObjectBindingField &Field) noexcept {
            return Field.getKind() == ObjectBindingFieldKind::Identifier;
        }

        [[nodiscard]] constexpr
        static auto classof(const ObjectBindingField *const Field) noexcept {
            return IsOfKind(*Field);
        }
    };

    struct ArrayBindingItem;
    struct ObjectBindingFieldArray : public ObjectBindingField {
        Qualifiers Qualifiers;
        std::vector<ArrayBindingItem *> ItemList;

        explicit
        ObjectBindingFieldArray(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const struct Qualifiers &Qualifiers,
            const std::span<ArrayBindingItem *> ItemList) noexcept
        : ObjectBindingField(ObjectBindingFieldKind::Array, Key, KeyLoc),
          Qualifiers(Qualifiers), ItemList(ItemList.begin(), ItemList.end()) {}

        explicit
        ObjectBindingFieldArray(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const struct Qualifiers &Qualifiers,
            std::vector<ArrayBindingItem *> &&ItemList) noexcept
        : ObjectBindingField(ObjectBindingFieldKind::Array, Key, KeyLoc),
          Qualifiers(Qualifiers), ItemList(std::move(ItemList)) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ObjectBindingField &Field) noexcept {
            return Field.getKind() == ObjectBindingFieldKind::Array;
        }

        [[nodiscard]] constexpr
        static auto classof(const ObjectBindingField *const Field) noexcept {
            return IsOfKind(*Field);
        }
    };

    struct ObjectBindingFieldObject : public ObjectBindingField {
        Qualifiers Qualifiers;
        std::vector<ObjectBindingField *> FieldList;

        explicit
        ObjectBindingFieldObject(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const struct Qualifiers &Qualifiers,
            const std::span<ObjectBindingField *> FieldList) noexcept
        : ObjectBindingField(ObjectBindingFieldKind::Object, Key, KeyLoc),
          Qualifiers(Qualifiers),
          FieldList(FieldList.begin(), FieldList.end()) {}

        explicit
        ObjectBindingFieldObject(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const struct Qualifiers &Qualifiers,
            std::vector<ObjectBindingField *> &&FieldList) noexcept
        : ObjectBindingField(ObjectBindingFieldKind::Object, Key, KeyLoc),
          Qualifiers(Qualifiers), FieldList(std::move(FieldList)) {}

        explicit
        ObjectBindingFieldObject(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            struct Qualifiers &&Qualifiers,
            const std::span<ObjectBindingField *> FieldList) noexcept
        : ObjectBindingField(ObjectBindingFieldKind::Object, Key, KeyLoc),
          Qualifiers(std::move(Qualifiers)),
          FieldList(FieldList.begin(), FieldList.end()) {}

        explicit
        ObjectBindingFieldObject(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            struct Qualifiers &&Qualifiers,
            std::vector<ObjectBindingField *> &&FieldList) noexcept
        : ObjectBindingField(ObjectBindingFieldKind::Object, Key, KeyLoc),
          Qualifiers(std::move(Qualifiers)), FieldList(std::move(FieldList)) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ObjectBindingField &Field) noexcept {
            return Field.getKind() == ObjectBindingFieldKind::Object;
        }

        [[nodiscard]] constexpr
        static auto classof(const ObjectBindingField *const Field) noexcept {
            return IsOfKind(*Field);
        }
    };

    struct ObjectBindingFieldSpread : public ObjectBindingField {
        Qualifiers Qualifiers;
        SourceLocation SpreadLoc;

        explicit
        ObjectBindingFieldSpread(const std::string_view Key,
                                 const SourceLocation KeyLoc,
                                 const SourceLocation SpreadLoc,
                                 const struct Qualifiers &Qualifiers) noexcept
        : ObjectBindingField(ObjectBindingFieldKind::Spread, Key, KeyLoc),
          Qualifiers(Qualifiers), SpreadLoc(SpreadLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ObjectBindingField &Field) noexcept {
            return Field.getKind() == ObjectBindingFieldKind::Spread;
        }

        [[nodiscard]] constexpr
        static auto classof(const ObjectBindingField *const Field) noexcept {
            return IsOfKind(*Field);
        }
    };

    struct ObjectBindingVarDecl : public Stmt {
    public:
        constexpr static auto ObjKind = NodeKind::ObjectBindingVarDecl;
    protected:
        SourceLocation Loc;
        Qualifiers Qualifiers;

        std::vector<ObjectBindingField *> FieldList;
        Expr *InitExpr;

        explicit
        ObjectBindingVarDecl(const NodeKind NodeKind,
                             const SourceLocation Loc,
                             const struct Qualifiers &Qualifiers,
                             const std::span<ObjectBindingField *> FieldList,
                             Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Qualifiers(Qualifiers),
          FieldList(FieldList.begin(), FieldList.end()), InitExpr(InitExpr) {}

        explicit
        ObjectBindingVarDecl(const NodeKind NodeKind,
                             const SourceLocation Loc,
                             const struct Qualifiers &Qualifiers,
                             std::vector<ObjectBindingField *> &&FieldList,
                             Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Qualifiers(Qualifiers),
          FieldList(std::move(FieldList)), InitExpr(InitExpr) {}

        explicit
        ObjectBindingVarDecl(const NodeKind NodeKind,
                             const SourceLocation Loc,
                             struct Qualifiers &&Qualifiers,
                             const std::span<ObjectBindingField *> FieldList,
                             Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Qualifiers(std::move(Qualifiers)),
          FieldList(FieldList.begin(), FieldList.end()), InitExpr(InitExpr) {}

        explicit
        ObjectBindingVarDecl(const NodeKind NodeKind,
                             const SourceLocation Loc,
                             struct Qualifiers &&Qualifiers,
                             std::vector<ObjectBindingField *> &&FieldList,
                             Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Qualifiers(std::move(Qualifiers)),
          FieldList(std::move(FieldList)), InitExpr(InitExpr) {}
    public:
        explicit
        ObjectBindingVarDecl(const SourceLocation Loc,
                             const struct Qualifiers &Qualifiers,
                             const std::span<ObjectBindingField *> FieldList,
                             Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(NodeKind::ObjectBindingVarDecl, Loc, Qualifiers,
                               FieldList, InitExpr) {}

        explicit
        ObjectBindingVarDecl(const SourceLocation Loc,
                             const struct Qualifiers &Qualifiers,
                             std::vector<ObjectBindingField *> &&FieldList,
                             Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(NodeKind::ObjectBindingVarDecl, Loc, Qualifiers,
                               FieldList, InitExpr) {}

        explicit
        ObjectBindingVarDecl(const SourceLocation Loc,
                             struct Qualifiers &&Qualifiers,
                             const std::span<ObjectBindingField *> FieldList,
                             Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(NodeKind::ObjectBindingVarDecl, Loc, Qualifiers,
                               FieldList, InitExpr) {}

        explicit
        ObjectBindingVarDecl(const SourceLocation Loc,
                             struct Qualifiers &&Qualifiers,
                             std::vector<ObjectBindingField *> &&FieldList,
                             Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(NodeKind::ObjectBindingVarDecl, Loc, Qualifiers,
                               FieldList, InitExpr) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind ||
                   Stmt.getKind() == NodeKind::ObjectBindingParamVarDecl;
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
