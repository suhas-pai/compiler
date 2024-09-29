/*
 * Sema/Types/Qualified.h
 * © suhas pai
 */

#pragma once

#include "ADT/PointerIntPair.h"
#include "Type.h"

namespace Sema {
    struct QualifiedType {
    protected:
        ADT::PointerIntPair<Type, uint32_t, TypeQualifierBits> TyAndBits;
    public:
        constexpr explicit
        QualifiedType(Type *const Type, const TypeQualifiers Qual) noexcept
        : TyAndBits(Type, Qual.getBits()) {}

        [[nodiscard]] constexpr auto getType() const noexcept {
            return TyAndBits.getPointer();
        }

        [[nodiscard]] constexpr auto getQualifiers() const noexcept {
            return TypeQualifiers(TyAndBits.getBits());
        }

        [[nodiscard]] constexpr auto isMutable() const noexcept {
            return getQualifiers().isMutable();
        }

        [[nodiscard]] constexpr auto isVolatile() const noexcept {
            return getQualifiers().isVolatile();
        }
    };
}