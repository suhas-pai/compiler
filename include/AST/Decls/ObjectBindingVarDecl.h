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
    private:
        ObjectBindingFieldKind Kind;
    protected:
        constexpr explicit
        ObjectBindingField(const ObjectBindingFieldKind Kind,
                           const std::string_view Key,
                           const SourceLocation KeyLoc) noexcept
        : Kind(Kind), Key(Key), KeyLoc(KeyLoc) {}
    public:
        [[nodiscard]] constexpr auto getKind() const noexcept {
            return this->Kind;
        }

        std::string_view Key;
        SourceLocation KeyLoc;
    };

    struct ObjectBindingFieldIdentifier : public ObjectBindingField {
        constexpr static auto ObjKind = ObjectBindingFieldKind::Identifier;
        Qualifiers Quals;

        std::string_view Name;
        SourceLocation NameLoc;

        explicit
        ObjectBindingFieldIdentifier(const std::string_view Key,
                                     const SourceLocation KeyLoc,
                                     const std::string_view Name,
                                     const SourceLocation NameLoc,
                                     const Qualifiers &Quals) noexcept
        : ObjectBindingField(ObjKind, Key, KeyLoc), Quals(Quals),
          Name(Name), NameLoc(NameLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ObjectBindingField &Field) noexcept {
            return Field.getKind() == ObjKind;
        }

        [[nodiscard]] constexpr
        static auto classof(const ObjectBindingField *const Field) noexcept {
            return IsOfKind(*Field);
        }
    };

    struct ArrayBindingItem;
    struct ObjectBindingFieldArray : public ObjectBindingField {
        constexpr static auto ObjKind = ObjectBindingFieldKind::Array;

        Qualifiers Quals;
        std::vector<ArrayBindingItem *> ItemList;

        explicit
        ObjectBindingFieldArray(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const Qualifiers &Quals,
            const std::span<ArrayBindingItem *> ItemList) noexcept
        : ObjectBindingField(ObjKind, Key, KeyLoc), Quals(Quals),
          ItemList(ItemList.begin(), ItemList.end()) {}

        explicit
        ObjectBindingFieldArray(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const Qualifiers &Quals,
            std::vector<ArrayBindingItem *> &&ItemList) noexcept
        : ObjectBindingField(ObjKind, Key, KeyLoc), Quals(Quals),
          ItemList(std::move(ItemList)) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ObjectBindingField &Field) noexcept {
            return Field.getKind() == ObjKind;
        }

        [[nodiscard]] constexpr
        static auto classof(const ObjectBindingField *const Field) noexcept {
            return IsOfKind(*Field);
        }
    };

    struct ObjectBindingFieldObject : public ObjectBindingField {
        constexpr static auto ObjKind = ObjectBindingFieldKind::Object;

        Qualifiers Quals;
        std::vector<ObjectBindingField *> FieldList;

        explicit
        ObjectBindingFieldObject(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const Qualifiers &Quals,
            const std::span<ObjectBindingField *> FieldList) noexcept
        : ObjectBindingField(ObjKind, Key, KeyLoc), Quals(Quals),
          FieldList(FieldList.begin(), FieldList.end()) {}

        explicit
        ObjectBindingFieldObject(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            const Qualifiers &Quals,
            std::vector<ObjectBindingField *> &&FieldList) noexcept
        : ObjectBindingField(ObjKind, Key, KeyLoc), Quals(Quals),
          FieldList(std::move(FieldList)) {}

        explicit
        ObjectBindingFieldObject(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            Qualifiers &&Quals,
            const std::span<ObjectBindingField *> FieldList) noexcept
        : ObjectBindingField(ObjKind, Key, KeyLoc),
          Quals(std::move(Quals)),
          FieldList(FieldList.begin(), FieldList.end()) {}

        explicit
        ObjectBindingFieldObject(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            Qualifiers &&Quals,
            std::vector<ObjectBindingField *> &&FieldList) noexcept
        : ObjectBindingField(ObjKind, Key, KeyLoc),
          Quals(std::move(Quals)), FieldList(std::move(FieldList)) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ObjectBindingField &Field) noexcept {
            return Field.getKind() == ObjKind;
        }

        [[nodiscard]] constexpr
        static auto classof(const ObjectBindingField *const Field) noexcept {
            return IsOfKind(*Field);
        }
    };

    struct ObjectBindingFieldSpread : public ObjectBindingField {
        Qualifiers Quals;
        SourceLocation SpreadLoc;

        constexpr static auto ObjKind = ObjectBindingFieldKind::Spread;

        explicit
        ObjectBindingFieldSpread(const std::string_view Key,
                                 const SourceLocation KeyLoc,
                                 const SourceLocation SpreadLoc,
                                 const Qualifiers &Quals) noexcept
        : ObjectBindingField(ObjKind, Key, KeyLoc), Quals(Quals),
          SpreadLoc(SpreadLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ObjectBindingField &Field) noexcept {
            return Field.getKind() == ObjKind;
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
        Qualifiers Quals;

        std::vector<ObjectBindingField *> FieldList;
        Expr *InitExpr;

        explicit
        ObjectBindingVarDecl(const NodeKind NodeKind,
                             const SourceLocation Loc,
                             const Qualifiers &Quals,
                             const std::span<ObjectBindingField *> FieldList,
                             Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Quals(Quals),
          FieldList(FieldList.begin(), FieldList.end()), InitExpr(InitExpr) {}

        explicit
        ObjectBindingVarDecl(const NodeKind NodeKind,
                             const SourceLocation Loc,
                             const Qualifiers &Quals,
                             std::vector<ObjectBindingField *> &&FieldList,
                             Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Quals(Quals),
          FieldList(std::move(FieldList)), InitExpr(InitExpr) {}

        explicit
        ObjectBindingVarDecl(const NodeKind NodeKind,
                             const SourceLocation Loc,
                             Qualifiers &&Quals,
                             const std::span<ObjectBindingField *> FieldList,
                             Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Quals(std::move(Quals)),
          FieldList(FieldList.begin(), FieldList.end()), InitExpr(InitExpr) {}

        explicit
        ObjectBindingVarDecl(const NodeKind NodeKind,
                             const SourceLocation Loc,
                             Qualifiers &&Quals,
                             std::vector<ObjectBindingField *> &&FieldList,
                             Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Quals(std::move(Quals)),
          FieldList(std::move(FieldList)), InitExpr(InitExpr) {}
    public:
        explicit
        ObjectBindingVarDecl(const SourceLocation Loc,
                             const Qualifiers &Quals,
                             const std::span<ObjectBindingField *> FieldList,
                             Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(ObjKind, Loc, Quals, FieldList, InitExpr) {}

        explicit
        ObjectBindingVarDecl(const SourceLocation Loc,
                             const Qualifiers &Quals,
                             std::vector<ObjectBindingField *> &&FieldList,
                             Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(ObjKind, Loc, Quals, FieldList, InitExpr) {}

        explicit
        ObjectBindingVarDecl(const SourceLocation Loc,
                             Qualifiers &&Quals,
                             const std::span<ObjectBindingField *> FieldList,
                             Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(ObjKind, Loc, Quals, FieldList, InitExpr) {}

        explicit
        ObjectBindingVarDecl(const SourceLocation Loc,
                             Qualifiers &&Quals,
                             std::vector<ObjectBindingField *> &&FieldList,
                             Expr *const InitExpr) noexcept
        : ObjectBindingVarDecl(ObjKind, Loc, Quals, FieldList, InitExpr) {}

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
            return this->Quals;
        }

        [[nodiscard]] constexpr auto &getQualifiers() noexcept {
            return this->Quals;
        }

        [[nodiscard]] constexpr auto getInitExpr() const noexcept {
            return this->InitExpr;
        }
    };
}
