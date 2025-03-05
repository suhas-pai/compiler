/*
 * Sema/Types/Type.h
 * © suhas pai
 */

#pragma once
#include <string_view>

#include "Qualifiers.h"
#include "Kind.h"

namespace Sema {
    struct alignas(TypeAlignment) Type {
    private:
        TypeKind Kind;
    protected:
        constexpr explicit Type(const TypeKind Kind) noexcept : Kind(Kind) {}
    public:
        [[nodiscard]] constexpr auto getKind() const noexcept {
            return this->Kind;
        }

        virtual constexpr std::string_view getName() const noexcept = 0;
    };
}
