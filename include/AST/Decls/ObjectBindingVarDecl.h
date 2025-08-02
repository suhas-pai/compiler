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
        std::string_view Key;
        SourceLocation KeyLoc;

        constexpr explicit
        ObjectBindingField(const ObjectBindingFieldKind Kind,
                           const std::string_view Key,
                           const SourceLocation KeyLoc) noexcept
        : Kind(Kind), Key(Key), KeyLoc(KeyLoc) {}
    public:
        [[nodiscard]] constexpr auto getKind() const noexcept {
            return this->Kind;
        }

        [[nodiscard]] constexpr auto getKey() const noexcept {
            return this->Key;
        }

        [[nodiscard]] constexpr auto getKeyLoc() const noexcept {
            return this->KeyLoc;
        }

        constexpr auto setKey(const std::string_view Key) noexcept
            -> decltype(*this)
        {
            this->Key = Key;
            return *this;
        }

        constexpr auto setKeyLoc(const SourceLocation KeyLoc) noexcept
            -> decltype(*this)
        {
            this->KeyLoc = KeyLoc;
            return *this;
        }
    };

    struct ObjectBindingFieldIdentifier : public ObjectBindingField {
    public:
        constexpr static auto ObjKind = ObjectBindingFieldKind::Identifier;
    protected:
        Qualifiers Quals;

        std::string_view Name;
        SourceLocation NameLoc;
    public:
        explicit
        ObjectBindingFieldIdentifier(const std::string_view Key,
                                     const SourceLocation KeyLoc,
                                     const std::string_view Name,
                                     const SourceLocation NameLoc,
                                     const Qualifiers &Quals) noexcept
        : ObjectBindingField(ObjKind, Key, KeyLoc), Quals(Quals),
          Name(Name), NameLoc(NameLoc) {}

        explicit
        ObjectBindingFieldIdentifier(const std::string_view Key,
                                     const SourceLocation KeyLoc,
                                     const std::string_view Name,
                                     const SourceLocation NameLoc,
                                     Qualifiers &&Quals) noexcept
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

        [[nodiscard]] constexpr auto getName() const noexcept {
            return this->Name;
        }

        [[nodiscard]] constexpr auto getNameLoc() const noexcept {
            return this->NameLoc;
        }

        [[nodiscard]] auto &getQualifiers() const noexcept {
            return this->Quals;
        }

        [[nodiscard]] auto &getQualifiersRef() noexcept {
            return this->Quals;
        }

        constexpr auto setName(const std::string_view Name) noexcept
            -> decltype(*this)
        {
            this->Name = Name;
            return *this;
        }

        constexpr auto setNameLoc(const SourceLocation NameLoc) noexcept
            -> decltype(*this)
        {
            this->NameLoc = NameLoc;
            return *this;
        }
    };

    struct ArrayBindingItem;
    struct ObjectBindingFieldArray : public ObjectBindingField {
    public:
        constexpr static auto ObjKind = ObjectBindingFieldKind::Array;
    protected:
        Qualifiers Quals;
        std::vector<ArrayBindingItem *> ItemList;
    public:
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

        explicit
        ObjectBindingFieldArray(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            Qualifiers &&Quals,
            const std::span<ArrayBindingItem *> ItemList) noexcept
        : ObjectBindingField(ObjKind, Key, KeyLoc), Quals(Quals),
          ItemList(ItemList.begin(), ItemList.end()) {}

        explicit
        ObjectBindingFieldArray(
            const std::string_view Key,
            const SourceLocation KeyLoc,
            Qualifiers &&Quals,
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

        [[nodiscard]] constexpr auto getItemList() const noexcept {
            return std::span(this->ItemList);
        }

        [[nodiscard]] constexpr auto &getItemListRef() noexcept {
            return this->ItemList;
        }

        [[nodiscard]] auto &getQualifiers() const noexcept {
            return this->Quals;
        }

        [[nodiscard]] auto &getQualifiersRef() noexcept {
            return this->Quals;
        }
    };

    struct ObjectBindingFieldObject : public ObjectBindingField {
    public:
        constexpr static auto ObjKind = ObjectBindingFieldKind::Object;
    protected:
        Qualifiers Quals;
        std::vector<ObjectBindingField *> FieldList;
    public:
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

        [[nodiscard]] constexpr auto getFieldList() const noexcept {
            return std::span(this->FieldList);
        }

        [[nodiscard]] constexpr auto &getFieldListRef() noexcept {
            return this->FieldList;
        }

        [[nodiscard]] auto &getQualifiers() const noexcept {
            return this->Quals;
        }

        [[nodiscard]] auto &getQualifiersRef() noexcept {
            return this->Quals;
        }
    };

    struct ObjectBindingFieldSpread : public ObjectBindingField {
    public:
        constexpr static auto ObjKind = ObjectBindingFieldKind::Spread;
    protected:
        Qualifiers Quals;
        SourceLocation SpreadLoc;
    public:
        explicit
        ObjectBindingFieldSpread(const std::string_view Key,
                                 const SourceLocation KeyLoc,
                                 const SourceLocation SpreadLoc,
                                 const Qualifiers &Quals) noexcept
        : ObjectBindingField(ObjKind, Key, KeyLoc), Quals(Quals),
          SpreadLoc(SpreadLoc) {}

        explicit
        ObjectBindingFieldSpread(const std::string_view Key,
                                 const SourceLocation KeyLoc,
                                 const SourceLocation SpreadLoc,
                                 Qualifiers &&Quals) noexcept
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

        [[nodiscard]] constexpr auto getSpreadLoc() const noexcept {
            return this->SpreadLoc;
        }

        [[nodiscard]] auto &getQualifiers() const noexcept {
            return this->Quals;
        }

        [[nodiscard]] auto &getQualifiersRef() noexcept {
            return this->Quals;
        }

        constexpr auto setSpreadLoc(const SourceLocation SpreadLoc) noexcept
            -> decltype(*this)
        {
            this->SpreadLoc = SpreadLoc;
            return *this;
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

        [[nodiscard]] auto &getQualifiers() const noexcept {
            return this->Quals;
        }

        [[nodiscard]] auto &getQualifiers() noexcept {
            return this->Quals;
        }

        [[nodiscard]] constexpr auto getInitExpr() const noexcept {
            return this->InitExpr;
        }
    };
}
