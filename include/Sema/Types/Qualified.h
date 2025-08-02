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
            return this->TyAndBits.getPointer();
        }

        [[nodiscard]] auto &getQualifiers() const noexcept {
            return TypeQualifiers(this->TyAndBits.getBits());
        }

        [[nodiscard]] constexpr auto isMutable() const noexcept {
            return this->getQualifiers().isMutable();
        }

        [[nodiscard]] constexpr auto isVolatile() const noexcept {
            return this->getQualifiers().isVolatile();
        }
    };
}