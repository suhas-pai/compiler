/*
 * AST/Decls/LvalueTypedDecl.h
 * © suhas pai
 */

#pragma once
#include <variant>

#include "AST/TypeRef.h"
#include "Sema/Types/Type.h"

#include "LvalueNamedDecl.h"

namespace AST {
    struct LvalueTypedDecl : public LvalueNamedDecl {
    protected:
        std::variant<Sema::Type *, TypeRef *> TypeOrTypeRef;
    public:
        constexpr explicit
        LvalueTypedDecl(const NodeKind ObjKind,
                        const std::string_view Name,
                        const SourceLocation NameLoc,
                        Expr *const RvalueExpr,
                        TypeRef *const TypeRef) noexcept
        : LvalueNamedDecl(ObjKind, Name, NameLoc, RvalueExpr),
          TypeOrTypeRef(TypeRef) {}

        constexpr explicit
        LvalueTypedDecl(const NodeKind ObjKind,
                        const std::string_view Name,
                        const SourceLocation NameLoc,
                        Expr *const RvalueExpr,
                        Sema::Type *const Type) noexcept
        : LvalueNamedDecl(ObjKind, Name, NameLoc, RvalueExpr),
          TypeOrTypeRef(Type) {}

        constexpr explicit
        LvalueTypedDecl(const NodeKind ObjKind,
                        const std::string &&Name,
                        const SourceLocation NameLoc,
                        Expr *const RvalueExpr,
                        TypeRef *const TypeRef) noexcept
        : LvalueNamedDecl(ObjKind, Name, NameLoc, RvalueExpr),
          TypeOrTypeRef(TypeRef) {}

        constexpr explicit
        LvalueTypedDecl(const NodeKind ObjKind,
                        const std::string &&Name,
                        const SourceLocation NameLoc,
                        Expr *const RvalueExpr,
                        Sema::Type *const Type) noexcept
        : LvalueNamedDecl(ObjKind, Name, NameLoc, RvalueExpr),
          TypeOrTypeRef(Type) {}

        [[nodiscard]] constexpr static auto IsOfKind(const Stmt &Stmt) {
            return Stmt.getKind() >= NodeKind::LvalueTypedDeclBase
                && Stmt.getKind() <= NodeKind::LvalueTypedDeclLast;
        }

        [[nodiscard]]
        constexpr static auto classof(const Stmt *const Node) noexcept {
            return IsOfKind(*Node);
        }

        [[nodiscard]] constexpr auto getTypeRef() const noexcept {
            return std::get<TypeRef *>(TypeOrTypeRef);
        }

        [[nodiscard]] constexpr auto getType() const noexcept {
            return std::get<Sema::Type *>(TypeOrTypeRef);
        }

        [[nodiscard]] constexpr auto hasInferredType() const noexcept {
            return this->getTypeRef() == nullptr;
        }

        constexpr auto setTypeRef(TypeRef *const TypeRef) noexcept
            -> decltype(*this)
        {
            this->TypeOrTypeRef = TypeRef;
            return *this;
        }

        constexpr auto setType(Sema::Type *const Type) noexcept
            -> decltype(*this)
        {
            this->TypeOrTypeRef = Type;
            return *this;
        }
    };
}
