/*
 * AST/VarQualifiers.h
 * © suhas pai
 */

#pragma once

namespace AST {
    struct VarQualifiers {
    protected:
        bool IsMutable : 1 = false;
        bool IsVolatile : 1 = false;
        bool IsComptime : 1 = false;
    public:
        constexpr explicit VarQualifiers() noexcept = default;

        [[nodiscard]] constexpr auto isMutable() const noexcept {
            return IsMutable;
        }

        [[nodiscard]] constexpr auto isVolatile() const noexcept {
            return IsVolatile;
        }

        [[nodiscard]] constexpr auto isComptime() const noexcept {
            return IsComptime;
        }

        constexpr auto setIsMutable(const bool IsMutable) noexcept
            -> decltype(*this)
        {
            this->IsMutable = IsMutable;
            return *this;
        }

        constexpr auto setIsVolatile(const bool IsVolatile) noexcept
            -> decltype(*this)
        {
            this->IsVolatile = IsVolatile;
            return *this;
        }

        constexpr auto setIsComptime(const bool IsComptime) noexcept
            -> decltype(*this)
        {
            this->IsComptime = IsComptime;
            return *this;
        }
    };
}
