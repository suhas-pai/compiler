/*
 * AST/Decls/ValueDecl.h
 * © suhas pai
 */

#pragma once

#include "AST/Decls/NamedDecl.h"
#include "AST/NodeKind.h"

namespace AST {

    struct ValueDecl : public NamedDecl {
    public:
        enum class Linkage {
            Private,
            Exported,
            External,
        };
    protected:
        std::variant<Sema::Type *, TypeRef *> TypeOrTypeRef;
        Linkage Link;
    public:
        constexpr explicit
        ValueDecl(const NodeKind ObjKind,
                  const std::string_view Name,
                  const SourceLocation NameLoc,
                  const Linkage Linkage,
                  TypeRef *const TypeRef) noexcept
        : NamedDecl(ObjKind, Name, NameLoc), TypeOrTypeRef(TypeRef),
          Link(Linkage) {}

        constexpr explicit
        ValueDecl(const NodeKind ObjKind,
                  const std::string_view Name,
                  const SourceLocation NameLoc,
                  const Linkage Linkage,
                  Sema::Type *const Type) noexcept
        : NamedDecl(ObjKind, Name, NameLoc), TypeOrTypeRef(Type),
          Link(Linkage) {}

        [[nodiscard]] constexpr auto getSemaType() const noexcept {
            return std::get<Sema::Type *>(TypeOrTypeRef);
        }

        [[nodiscard]] constexpr auto getTypeRef() const noexcept {
            return std::get<TypeRef *>(TypeOrTypeRef);
        }

        [[nodiscard]] constexpr auto hasSemaType() const noexcept {
            return std::holds_alternative<Sema::Type *>(TypeOrTypeRef);
        }

        [[nodiscard]] constexpr auto hasInferredType() const noexcept {
            return std::holds_alternative<TypeRef *>(TypeOrTypeRef)
                && std::get<TypeRef *>(TypeOrTypeRef) == nullptr;
        }

        [[nodiscard]] constexpr auto getLinkage() const noexcept {
            return Link;
        }

        constexpr auto setSemaType(Sema::Type *const Type) noexcept
            -> decltype(*this)
        {
            this->TypeOrTypeRef = Type;
            return *this;
        }

        constexpr auto setTypeRef(TypeRef *const TypeRef) noexcept
            -> decltype(*this)
        {
            this->TypeOrTypeRef = TypeRef;
            return *this;
        }

        constexpr
        auto setLinkage(const Linkage Link) noexcept -> decltype(*this) {
            this->Link = Link;
            return *this;
        }
    };
}