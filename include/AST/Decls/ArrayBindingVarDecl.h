/*
 * AST/Decls/ArrayBindingVarDecl.h
 * © suhas pai
 */

#pragma once

#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include "AST/Expr.h"
#include "AST/Qualifiers.h"

namespace AST {
    enum class ArrayBindingItemKind {
        Identifier,
        Array,
        Object,
        Spread,
    };

    struct ArrayBindingIndex {
        Expr *IndexExpr;
        SourceLocation IndexLoc;

        constexpr explicit
        ArrayBindingIndex(Expr *const IndexExpr,
                          const SourceLocation IndexLoc) noexcept
        : IndexExpr(IndexExpr), IndexLoc(IndexLoc) {}
    };

    struct ArrayBindingItem {
    private:
        ArrayBindingItemKind Kind;
    protected:
        std::optional<ArrayBindingIndex> Index;
        Qualifiers Quals;

        explicit
        ArrayBindingItem(const ArrayBindingItemKind Kind,
                         const Qualifiers &Quals,
                         const std::optional<ArrayBindingIndex> Index) noexcept
        : Kind(Kind), Index(Index), Quals(Quals) {}

        explicit
        ArrayBindingItem(const ArrayBindingItemKind Kind,
                         Qualifiers &&Quals,
                         const std::optional<ArrayBindingIndex> Index) noexcept
        : Kind(Kind), Index(Index), Quals(std::move(Quals)) {}
    public:
        [[nodiscard]] constexpr auto getKind() const noexcept {
            return this->Kind;
        }

        [[nodiscard]] auto &getQualifiers() const noexcept {
            return this->Quals;
        }

        [[nodiscard]] auto &getQualifiersRef() noexcept {
            return this->Quals;
        }

        [[nodiscard]] constexpr auto getIndex() const noexcept {
            return this->Index;
        }

        constexpr
        auto setIndex(const std::optional<ArrayBindingIndex> Index) noexcept
            -> decltype(*this)
        {
            this->Index = Index;
            return *this;
        }
    };

    struct ArrayBindingItemIdentifier : public ArrayBindingItem {
    public:
        constexpr static auto ObjKind = ArrayBindingItemKind::Identifier;
    protected:
        std::string Name;
        SourceLocation NameLoc;
    public:
        explicit
        ArrayBindingItemIdentifier(const Qualifiers &Quals,
                                   const std::optional<ArrayBindingIndex> Index,
                                   const std::string_view Name,
                                   const SourceLocation NameLoc) noexcept
        : ArrayBindingItem(ObjKind, Quals, Index), Name(Name),
          NameLoc(NameLoc) {}

        explicit
        ArrayBindingItemIdentifier(Qualifiers &&Quals,
                                   const std::optional<ArrayBindingIndex> Index,
                                   const std::string_view Name,
                                   const SourceLocation NameLoc) noexcept
        : ArrayBindingItem(ObjKind, Quals, Index), Name(Name),
          NameLoc(NameLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ArrayBindingItem &Item) noexcept {
            return Item.getKind() == ObjKind;
        }

        [[nodiscard]] constexpr
        static auto classof(const ArrayBindingItem *const Item) noexcept {
            return IsOfKind(*Item);
        }

        [[nodiscard]] constexpr auto getName() const noexcept {
            return this->Name;
        }

        [[nodiscard]] constexpr auto getNameLoc() const noexcept {
            return this->NameLoc;
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

    struct ArrayBindingItemArray : public ArrayBindingItem {
    public:
        constexpr static auto ObjKind = ArrayBindingItemKind::Array;
    protected:
        std::vector<ArrayBindingItem *> ItemList;
        SourceLocation ItemLoc;
    public:
        explicit
        ArrayBindingItemArray(const Qualifiers &Quals,
                              const std::optional<ArrayBindingIndex> Index,
                              const std::span<ArrayBindingItem *> ItemList,
                              const SourceLocation ItemLoc) noexcept
        : ArrayBindingItem(ObjKind, Quals, Index),
          ItemList(ItemList.begin(), ItemList.end()), ItemLoc(ItemLoc) {}

        explicit
        ArrayBindingItemArray(const Qualifiers &Quals,
                              const std::optional<ArrayBindingIndex> Index,
                              std::vector<ArrayBindingItem *> &&ItemList,
                              const SourceLocation ItemLoc) noexcept
        : ArrayBindingItem(ObjKind, Quals, Index),
          ItemList(std::move(ItemList)), ItemLoc(ItemLoc) {}

        explicit
        ArrayBindingItemArray(Qualifiers &&Quals,
                              const std::optional<ArrayBindingIndex> Index,
                              const std::span<ArrayBindingItem *> ItemList,
                              const SourceLocation ItemLoc) noexcept
        : ArrayBindingItem(ObjKind, Quals, Index),
          ItemList(ItemList.begin(), ItemList.end()), ItemLoc(ItemLoc) {}

        explicit
        ArrayBindingItemArray(Qualifiers &&Quals,
                              const std::optional<ArrayBindingIndex> Index,
                              std::vector<ArrayBindingItem *> &&ItemList,
                              const SourceLocation ItemLoc) noexcept
        : ArrayBindingItem(ObjKind, Quals, Index),
          ItemList(std::move(ItemList)), ItemLoc(ItemLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ArrayBindingItem &Item) noexcept {
            return Item.getKind() == ObjKind;
        }

        [[nodiscard]] constexpr
        static auto classof(const ArrayBindingItem *const Item) noexcept {
            return IsOfKind(*Item);
        }

        [[nodiscard]] constexpr auto getItemList() const noexcept {
            return std::span(this->ItemList);
        }

        [[nodiscard]] constexpr auto &getItemListRef() noexcept {
            return this->ItemList;
        }

        [[nodiscard]] constexpr auto getItemLoc() const noexcept {
            return this->ItemLoc;
        }

        constexpr auto setItemLoc(const SourceLocation ItemLoc) noexcept
            -> decltype(*this)
        {
            this->ItemLoc = ItemLoc;
            return *this;
        }
    };

    struct ObjectBindingField;
    struct ArrayBindingItemObject : public ArrayBindingItem {
    public:
        constexpr static auto ObjKind = ArrayBindingItemKind::Object;
    protected:
        std::vector<ObjectBindingField *> FieldList;
        SourceLocation ItemLoc;
    public:
        explicit
        ArrayBindingItemObject(const Qualifiers &Quals,
                               const std::optional<ArrayBindingIndex> Index,
                               const std::span<ObjectBindingField *> FieldList,
                               const SourceLocation ItemLoc) noexcept
        : ArrayBindingItem(ObjKind, Quals, Index),
          FieldList(FieldList.begin(), FieldList.end()), ItemLoc(ItemLoc) {}

        explicit
        ArrayBindingItemObject(const Qualifiers &Quals,
                               const std::optional<ArrayBindingIndex> Index,
                               std::vector<ObjectBindingField *> &&FieldList,
                               const SourceLocation ItemLoc) noexcept
        : ArrayBindingItem(ObjKind, Quals, Index),
          FieldList(std::move(FieldList)), ItemLoc(ItemLoc) {}

        explicit
        ArrayBindingItemObject(Qualifiers &&Quals,
                               const std::optional<ArrayBindingIndex> Index,
                               const std::span<ObjectBindingField *> FieldList,
                               const SourceLocation ItemLoc) noexcept
        : ArrayBindingItem(ObjKind, Quals, Index),
          FieldList(FieldList.begin(), FieldList.end()), ItemLoc(ItemLoc) {}

        explicit
        ArrayBindingItemObject(Qualifiers &&Quals,
                               const std::optional<ArrayBindingIndex> Index,
                               std::vector<ObjectBindingField *> &&FieldList,
                               const SourceLocation ItemLoc) noexcept
        : ArrayBindingItem(ObjKind, Quals, Index),
          FieldList(std::move(FieldList)), ItemLoc(ItemLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ArrayBindingItem &Item) noexcept {
            return Item.getKind() == ObjKind;
        }

        [[nodiscard]] constexpr
        static auto classof(const ArrayBindingItem *const Item) noexcept {
            return IsOfKind(*Item);
        }

        [[nodiscard]] constexpr auto getFieldList() const noexcept {
            return std::span(this->FieldList);
        }

        [[nodiscard]] constexpr auto &getFieldListRef() noexcept {
            return this->FieldList;
        }

        [[nodiscard]] constexpr auto getItemLoc() const noexcept {
            return this->ItemLoc;
        }

        constexpr auto setItemLoc(const SourceLocation ItemLoc) noexcept
            -> decltype(*this)
        {
            this->ItemLoc = ItemLoc;
            return *this;
        }
    };

    struct ArrayBindingItemSpread : public ArrayBindingItem {
    public:
        constexpr static auto ObjKind = ArrayBindingItemKind::Spread;
    protected:
        SourceLocation SpreadLoc;

        std::string_view Name;
        SourceLocation NameLoc;
    public:
        explicit
        ArrayBindingItemSpread(const Qualifiers &Quals,
                               const std::optional<ArrayBindingIndex> Index,
                               const std::string_view Name,
                               const SourceLocation NameLoc,
                               const SourceLocation SpreadLoc) noexcept
        : ArrayBindingItem(ObjKind, Quals, Index), SpreadLoc(SpreadLoc),
          Name(Name), NameLoc(NameLoc) {}

        explicit
        ArrayBindingItemSpread(Qualifiers &&Quals,
                               const std::optional<ArrayBindingIndex> Index,
                               const std::string_view Name,
                               const SourceLocation NameLoc,
                               const SourceLocation SpreadLoc) noexcept
        : ArrayBindingItem(ObjKind, Quals, Index), SpreadLoc(SpreadLoc),
          Name(Name), NameLoc(NameLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ArrayBindingItem &Item) noexcept {
            return Item.getKind() == ObjKind;
        }

        [[nodiscard]] constexpr
        static auto classof(const ArrayBindingItem *const Item) noexcept {
            return IsOfKind(*Item);
        }

        [[nodiscard]] constexpr auto getSpreadLoc() const noexcept {
            return this->SpreadLoc;
        }

        [[nodiscard]] constexpr auto getName() const noexcept {
            return this->Name;
        }

        [[nodiscard]] constexpr auto getNameLoc() const noexcept {
            return this->NameLoc;
        }

        constexpr auto setSpreadLoc(const SourceLocation SpreadLoc) noexcept
            -> decltype(*this)
        {
            this->SpreadLoc = SpreadLoc;
            return *this;
        }
    };

    struct ArrayBindingVarDecl : public Stmt {
    public:
        constexpr static auto ObjKind = NodeKind::ArrayBindingVarDecl;
    protected:
        SourceLocation Loc;
        Qualifiers Quals;

        std::vector<ArrayBindingItem *> ItemList;
        Expr *InitExpr;

        explicit
        ArrayBindingVarDecl(const NodeKind NodeKind,
                            const SourceLocation Loc,
                            const Qualifiers &Quals,
                            const std::span<ArrayBindingItem *> ItemList,
                            Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Quals(Quals),
          ItemList(ItemList.begin(), ItemList.end()), InitExpr(InitExpr) {}

        explicit
        ArrayBindingVarDecl(const NodeKind NodeKind,
                            const SourceLocation Loc,
                            const Qualifiers &Quals,
                            std::vector<ArrayBindingItem *> &&ItemList,
                            Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Quals(Quals), ItemList(std::move(ItemList)),
          InitExpr(InitExpr) {}

        explicit
        ArrayBindingVarDecl(const NodeKind NodeKind,
                            const SourceLocation Loc,
                            Qualifiers &&Quals,
                            const std::span<ArrayBindingItem *> &ItemList,
                            Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Quals(Quals),
          ItemList(ItemList.begin(), ItemList.end()), InitExpr(InitExpr) {}

        explicit
        ArrayBindingVarDecl(const NodeKind NodeKind,
                            const SourceLocation Loc,
                            Qualifiers &&Quals,
                            std::vector<ArrayBindingItem *> &&ItemList,
                            Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Quals(Quals), ItemList(std::move(ItemList)),
          InitExpr(InitExpr) {}
    public:
        explicit
        ArrayBindingVarDecl(const SourceLocation Loc,
                            const Qualifiers &Quals,
                            const std::span<ArrayBindingItem *> ItemList,
                            Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Quals, ItemList, InitExpr) {}

        explicit
        ArrayBindingVarDecl(const SourceLocation Loc,
                            const Qualifiers &Quals,
                            std::vector<ArrayBindingItem *> &&ItemList,
                            Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Quals, ItemList, InitExpr) {}

        explicit
        ArrayBindingVarDecl(const SourceLocation Loc,
                            Qualifiers &&Quals,
                            const std::span<ArrayBindingItem *> &ItemList,
                            Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Quals, ItemList, InitExpr) {}

        explicit
        ArrayBindingVarDecl(const SourceLocation Loc,
                            Qualifiers &&Quals,
                            std::vector<ArrayBindingItem *> &&ItemList,
                            Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Quals, ItemList, InitExpr) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == ObjKind ||
                   Stmt.getKind() == NodeKind::ArrayBindingParamVarDecl;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Stmt) noexcept {
            return IsOfKind(*Stmt);
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

        [[nodiscard]] constexpr auto getInitExpr() const noexcept {
            return this->InitExpr;
        }

        constexpr auto setInitExpr(Expr *const Expr) noexcept -> decltype(*this)
        {
            this->InitExpr = Expr;
            return *this;
        }
    };
}
