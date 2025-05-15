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
        explicit
        ArrayBindingItem(const ArrayBindingItemKind Kind,
                         const struct Qualifiers &Qualifiers,
                         const std::optional<ArrayBindingIndex> Index) noexcept
        : Kind(Kind), Index(Index), Qualifiers(Qualifiers) {}
    public:
        std::optional<ArrayBindingIndex> Index;
        Qualifiers Qualifiers;

        [[nodiscard]] constexpr auto getKind() const noexcept {
            return this->Kind;
        }
    };

    struct ArrayBindingItemIdentifier : public ArrayBindingItem {
        constexpr static auto ObjKind = ArrayBindingItemKind::Identifier;

        std::string_view Name;
        SourceLocation NameLoc;

        explicit
        ArrayBindingItemIdentifier(const struct Qualifiers &Qualifiers,
                                   const std::optional<ArrayBindingIndex> Index,
                                   const std::string_view Name,
                                   const SourceLocation NameLoc) noexcept
        : ArrayBindingItem(ObjKind, Qualifiers, Index), Name(Name),
          NameLoc(NameLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ArrayBindingItem &Item) noexcept {
            return Item.getKind() == ObjKind;
        }

        [[nodiscard]] constexpr
        static auto classof(const ArrayBindingItem *const Item) noexcept {
            return IsOfKind(*Item);
        }
    };

    struct ArrayBindingItemArray : public ArrayBindingItem {
        constexpr static auto ObjKind = ArrayBindingItemKind::Array;

        std::vector<ArrayBindingItem *> ItemList;
        SourceLocation ItemLoc;

        explicit
        ArrayBindingItemArray(const struct Qualifiers &Qualifiers,
                              const std::optional<ArrayBindingIndex> Index,
                              const std::span<ArrayBindingItem *> ItemList,
                              const SourceLocation ItemLoc) noexcept
        : ArrayBindingItem(ObjKind, Qualifiers, Index),
          ItemList(ItemList.begin(), ItemList.end()), ItemLoc(ItemLoc) {}

        explicit
        ArrayBindingItemArray(const struct Qualifiers &Qualifiers,
                              const std::optional<ArrayBindingIndex> Index,
                              std::vector<ArrayBindingItem *> &&ItemList,
                              const SourceLocation ItemLoc) noexcept
        : ArrayBindingItem(ObjKind, Qualifiers, Index),
          ItemList(std::move(ItemList)), ItemLoc(ItemLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ArrayBindingItem &Item) noexcept {
            return Item.getKind() == ObjKind;
        }

        [[nodiscard]] constexpr
        static auto classof(const ArrayBindingItem *const Item) noexcept {
            return IsOfKind(*Item);
        }
    };

    struct ObjectBindingField;
    struct ArrayBindingItemObject : public ArrayBindingItem {
        constexpr static auto ObjKind = ArrayBindingItemKind::Object;

        std::vector<ObjectBindingField *> FieldList;
        SourceLocation ItemLoc;

        explicit
        ArrayBindingItemObject(const struct Qualifiers &Qualifiers,
                               const std::optional<ArrayBindingIndex> Index,
                               const std::span<ObjectBindingField *> FieldList,
                               const SourceLocation ItemLoc) noexcept
        : ArrayBindingItem(ObjKind, Qualifiers, Index),
          FieldList(FieldList.begin(), FieldList.end()), ItemLoc(ItemLoc) {}

        explicit
        ArrayBindingItemObject(const struct Qualifiers &Qualifiers,
                               const std::optional<ArrayBindingIndex> Index,
                               std::vector<ObjectBindingField *> &&FieldList,
                               const SourceLocation ItemLoc) noexcept
        : ArrayBindingItem(ObjKind, Qualifiers, Index),
          FieldList(std::move(FieldList)), ItemLoc(ItemLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ArrayBindingItem &Item) noexcept {
            return Item.getKind() == ObjKind;
        }

        [[nodiscard]] constexpr
        static auto classof(const ArrayBindingItem *const Item) noexcept {
            return IsOfKind(*Item);
        }
    };

    struct ArrayBindingItemSpread : public ArrayBindingItem {
        constexpr static auto ObjKind = ArrayBindingItemKind::Spread;
        SourceLocation SpreadLoc;

        std::string_view Name;
        SourceLocation NameLoc;

        explicit
        ArrayBindingItemSpread(const struct Qualifiers &Qualifiers,
                               const std::optional<ArrayBindingIndex> Index,
                               const std::string_view Name,
                               const SourceLocation NameLoc,
                               const SourceLocation SpreadLoc) noexcept
        : ArrayBindingItem(ObjKind, Qualifiers, Index), SpreadLoc(SpreadLoc),
          Name(Name), NameLoc(NameLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ArrayBindingItem &Item) noexcept {
            return Item.getKind() == ObjKind;
        }

        [[nodiscard]] constexpr
        static auto classof(const ArrayBindingItem *const Item) noexcept {
            return IsOfKind(*Item);
        }
    };

    struct ArrayBindingVarDecl : public Stmt {
    public:
        constexpr static auto ObjKind = NodeKind::ArrayBindingVarDecl;
    protected:
        SourceLocation Loc;
        Qualifiers Qualifiers;

        std::vector<ArrayBindingItem *> ItemList;
        Expr *InitExpr;

        explicit
        ArrayBindingVarDecl(const NodeKind NodeKind,
                            const SourceLocation Loc,
                            const struct Qualifiers &Qualifiers,
                            const std::span<ArrayBindingItem *> ItemList,
                            Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Qualifiers(Qualifiers),
          ItemList(ItemList.begin(), ItemList.end()), InitExpr(InitExpr) {}

        explicit
        ArrayBindingVarDecl(const NodeKind NodeKind,
                            const SourceLocation Loc,
                            const struct Qualifiers &Qualifiers,
                            std::vector<ArrayBindingItem *> &&ItemList,
                            Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Qualifiers(Qualifiers),
          ItemList(std::move(ItemList)), InitExpr(InitExpr) {}

        explicit
        ArrayBindingVarDecl(const NodeKind NodeKind,
                            const SourceLocation Loc,
                            struct Qualifiers &&Qualifiers,
                            const std::span<ArrayBindingItem *> &ItemList,
                            Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Qualifiers(Qualifiers),
          ItemList(ItemList.begin(), ItemList.end()), InitExpr(InitExpr) {}

        explicit
        ArrayBindingVarDecl(const NodeKind NodeKind,
                            const SourceLocation Loc,
                            struct Qualifiers &&Qualifiers,
                            std::vector<ArrayBindingItem *> &&ItemList,
                            Expr *const InitExpr) noexcept
        : Stmt(NodeKind), Loc(Loc), Qualifiers(Qualifiers),
          ItemList(std::move(ItemList)), InitExpr(InitExpr) {}
    public:
        explicit
        ArrayBindingVarDecl(const SourceLocation Loc,
                            const struct Qualifiers &Qualifiers,
                            const std::span<ArrayBindingItem *> ItemList,
                            Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Qualifiers, ItemList, InitExpr) {}

        explicit
        ArrayBindingVarDecl(const SourceLocation Loc,
                            const struct Qualifiers &Qualifiers,
                            std::vector<ArrayBindingItem *> &&ItemList,
                            Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Qualifiers, ItemList, InitExpr) {}

        explicit
        ArrayBindingVarDecl(const SourceLocation Loc,
                            struct Qualifiers &&Qualifiers,
                            const std::span<ArrayBindingItem *> &ItemList,
                            Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Qualifiers, ItemList, InitExpr) {}

        explicit
        ArrayBindingVarDecl(const SourceLocation Loc,
                            struct Qualifiers &&Qualifiers,
                            std::vector<ArrayBindingItem *> &&ItemList,
                            Expr *const InitExpr) noexcept
        : ArrayBindingVarDecl(ObjKind, Loc, Qualifiers, ItemList, InitExpr) {}

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

        [[nodiscard]] constexpr auto &getQualifiers() const noexcept {
            return this->Qualifiers;
        }

        [[nodiscard]] constexpr auto &getQualifiersRef() noexcept {
            return this->Qualifiers;
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
