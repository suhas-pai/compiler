/*
 * Sema/Types/Pointer.h
 * © suhas pai
 */

#pragma once
#include "Type.h"

namespace Sema {
    struct PointerType : public Type {
    public:
        constexpr static auto TyKind = TypeKind::Pointer;
    protected:
        Type *PointeeType;
    public:
        constexpr explicit PointerType(Type *const PointeeType) noexcept
        : Type(TypeKind::Pointer), PointeeType(PointeeType) {}

        [[nodiscard]]
        constexpr static inline auto IsOfKind(const Type &Ty) noexcept {
            return Ty.getKind() == TyKind;
        }

        [[nodiscard]]
        constexpr static inline auto classof(const Type *const Type) noexcept {
            return IsOfKind(*Type);
        }

        [[nodiscard]] constexpr auto getPointeeType() const noexcept {
            return PointeeType;
        }

        [[nodiscard]]
        constexpr std::string_view getName() const noexcept override {
            return PointeeType->getName();
        }
    };
}
