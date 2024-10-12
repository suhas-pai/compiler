/*
 * Sema/Types/Qualifiers.h
 * © suhas pai
 */

#pragma once
#include <cstdint>

namespace Sema {
    enum TypeQualifierKind {
        TypeQualifierMutable,
        TypeQualifierVolatile,
        TypeQualifierHasExternal,
    };

    // Store qualifiers in lower bits of pointers to Type
    constexpr auto TypeQualifierBits = 3;
    constexpr auto TypeAlignment = 1 << TypeQualifierBits;

    struct TypeQualifiers {
    protected:
        uint32_t Bits = 0;
    public:
        constexpr explicit TypeQualifiers() noexcept = default;

        constexpr
        explicit TypeQualifiers(const uint32_t Bits) noexcept : Bits(Bits) {}

        [[nodiscard]] constexpr auto getBits() const noexcept {
            return Bits;
        }

        [[nodiscard]] constexpr auto isMutable() const noexcept {
            return Bits & (1 << TypeQualifierMutable);
        }

        [[nodiscard]] constexpr auto isVolatile() const noexcept {
            return Bits & (1 << TypeQualifierVolatile);
        }

        constexpr auto setIsMutable(const bool IsMutable) noexcept
            -> decltype(*this)
        {
            Bits |= 1 << TypeQualifierMutable;
            return *this;
        }

        constexpr auto setIsVolatile(const bool IsVolatile) noexcept
            -> decltype(*this)
        {
            Bits |= 1 << TypeQualifierVolatile;
            return *this;
        }
    };
}
