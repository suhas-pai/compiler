/*
 * AST/Decls/ArrayDestructredVarDecl.h
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
    enum class ArrayDestructureItemKind {
        Identifier,
        Array,
        Object,
        Spread,
    };

    struct ArrayDestructureIndex {
        Expr *IndexExpr;
        SourceLocation IndexLoc;

        constexpr explicit
        ArrayDestructureIndex(Expr *const IndexExpr,
                              const SourceLocation IndexLoc) noexcept
        : IndexExpr(IndexExpr), IndexLoc(IndexLoc) {}
    };

    struct ArrayDestructureItem {
    protected:
        ArrayDestructureItemKind Kind;

        explicit
        ArrayDestructureItem(
            const ArrayDestructureItemKind Kind,
            const struct Qualifiers &Qualifiers,
            const std::optional<ArrayDestructureIndex> Index) noexcept
        : Kind(Kind), Index(Index), Qualifiers(Qualifiers) {}
    public:
        std::optional<ArrayDestructureIndex> Index;
        Qualifiers Qualifiers;

        [[nodiscard]] constexpr auto getKind() const noexcept {
            return this->Kind;
        }
    };

    struct ArrayDestructureItemIdentifier : public ArrayDestructureItem {
        std::string_view Name;
        SourceLocation NameLoc;

        explicit
        ArrayDestructureItemIdentifier(
            const struct Qualifiers &Qualifiers,
            const std::optional<ArrayDestructureIndex> Index,
            const std::string_view Name,
            const SourceLocation NameLoc) noexcept
        : ArrayDestructureItem(ArrayDestructureItemKind::Identifier, Qualifiers,
                               Index),
          Name(Name), NameLoc(NameLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ArrayDestructureItem &Item) noexcept {
            return Item.getKind() == ArrayDestructureItemKind::Identifier;
        }

        [[nodiscard]] constexpr
        static auto classof(const ArrayDestructureItem *const Item) noexcept {
            return IsOfKind(*Item);
        }
    };

    struct ArrayDestructureItemArray : public ArrayDestructureItem {
        std::vector<ArrayDestructureItem *> ItemList;
        SourceLocation ItemLoc;

        explicit
        ArrayDestructureItemArray(
            const struct Qualifiers &Qualifiers,
            const std::optional<ArrayDestructureIndex> Index,
            const std::span<ArrayDestructureItem *> ItemList,
            const SourceLocation ItemLoc) noexcept
        : ArrayDestructureItem(ArrayDestructureItemKind::Array, Qualifiers,
                               Index),
          ItemList(ItemList.begin(), ItemList.end()), ItemLoc(ItemLoc) {}

        explicit
        ArrayDestructureItemArray(
            const struct Qualifiers &Qualifiers,
            const std::optional<ArrayDestructureIndex> Index,
            std::vector<ArrayDestructureItem *> &&ItemList,
            const SourceLocation ItemLoc) noexcept
        : ArrayDestructureItem(ArrayDestructureItemKind::Array, Qualifiers,
                               Index),
          ItemList(std::move(ItemList)), ItemLoc(ItemLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ArrayDestructureItem &Item) noexcept {
            return Item.getKind() == ArrayDestructureItemKind::Array;
        }

        [[nodiscard]] constexpr
        static auto classof(const ArrayDestructureItem *const Item) noexcept {
            return IsOfKind(*Item);
        }
    };

    struct ObjectDestructureField;
    struct ArrayDestructureItemObject : public ArrayDestructureItem {
        std::vector<ObjectDestructureField *> FieldList;
        SourceLocation ItemLoc;

        explicit
        ArrayDestructureItemObject(
            const struct Qualifiers &Qualifiers,
            const std::optional<ArrayDestructureIndex> Index,
            const std::span<ObjectDestructureField *> FieldList,
            const SourceLocation ItemLoc) noexcept
        : ArrayDestructureItem(ArrayDestructureItemKind::Object, Qualifiers,
                               Index),
          FieldList(FieldList.begin(), FieldList.end()), ItemLoc(ItemLoc) {}

        explicit
        ArrayDestructureItemObject(
            const struct Qualifiers &Qualifiers,
            const std::optional<ArrayDestructureIndex> Index,
            std::vector<ObjectDestructureField *> &&FieldList,
            const SourceLocation ItemLoc) noexcept
        : ArrayDestructureItem(ArrayDestructureItemKind::Object, Qualifiers,
                               Index),
          FieldList(std::move(FieldList)), ItemLoc(ItemLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ArrayDestructureItem &Item) noexcept {
            return Item.getKind() == ArrayDestructureItemKind::Object;
        }

        [[nodiscard]] constexpr
        static auto classof(const ArrayDestructureItem *const Item) noexcept {
            return IsOfKind(*Item);
        }
    };

    struct ArrayDestructureItemSpread : public ArrayDestructureItem {
        SourceLocation SpreadLoc;

        std::string_view Name;
        SourceLocation NameLoc;

        explicit
        ArrayDestructureItemSpread(
            const struct Qualifiers &Qualifiers,
            const std::optional<ArrayDestructureIndex> Index,
            const std::string_view Name,
            const SourceLocation NameLoc,
            const SourceLocation SpreadLoc) noexcept
        : ArrayDestructureItem(ArrayDestructureItemKind::Spread, Qualifiers,
                               Index),
          SpreadLoc(SpreadLoc), Name(Name), NameLoc(NameLoc) {}

        [[nodiscard]] constexpr
        static auto IsOfKind(const ArrayDestructureItem &Item) noexcept {
            return Item.getKind() == ArrayDestructureItemKind::Spread;
        }

        [[nodiscard]] constexpr
        static auto classof(const ArrayDestructureItem *const Item) noexcept {
            return IsOfKind(*Item);
        }
    };

    struct ArrayDestructuredVarDecl : public Stmt {
    protected:
        Qualifiers Qualifiers;
        std::vector<ArrayDestructureItem *> ItemList;
        Expr *InitExpr;
    public:
        explicit
        ArrayDestructuredVarDecl(
            const SourceLocation Loc,
            const struct Qualifiers &Qualifiers,
            const std::span<ArrayDestructureItem *> ItemList,
            Expr *const InitExpr) noexcept
        : Stmt(NodeKind::ArrayDestructuredVarDecl), Qualifiers(Qualifiers),
          ItemList(ItemList.begin(), ItemList.end()), InitExpr(InitExpr) {}

        explicit
        ArrayDestructuredVarDecl(
            const SourceLocation Loc,
            const struct Qualifiers &Qualifiers,
            std::vector<ArrayDestructureItem *> &&ItemList,
            Expr *const InitExpr) noexcept
        : Stmt(NodeKind::ArrayDestructuredVarDecl), Qualifiers(Qualifiers),
          ItemList(std::move(ItemList)), InitExpr(InitExpr) {}

        [[nodiscard]]
        constexpr static auto IsOfKind(const Stmt &Stmt) noexcept {
            return Stmt.getKind() == NodeKind::ArrayDestructuredVarDecl;
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
