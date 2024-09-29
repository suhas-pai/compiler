/*
 * Sema/Types/Struct.h
 * © suhas pai
 */

#pragma once

#include <string>
#include "Type.h"

namespace Sema {
    struct StructType : public Type {
    public:
        constexpr static auto TyKind = TypeKind::Structure;
    protected:
        std::string Name;
    public:
        constexpr explicit StructType(const std::string_view Name) noexcept
        : Type(TypeKind::Structure), Name(Name) {}

        constexpr explicit StructType(std::string &&Name) noexcept
        : Type(TypeKind::Structure), Name(std::move(Name)) {}

        [[nodiscard]]
        constexpr std::string_view getName() const noexcept override {
            return Name;
        }
    };
}
