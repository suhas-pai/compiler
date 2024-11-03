/*
 * Sema/Types/Struct.h
 * © suhas pai
 */

#pragma once

#include <string>
#include <vector>

#include "Qualified.h"

namespace Sema {
    struct StructType : public Type {
    public:
        constexpr static auto TyKind = TypeKind::Structure;

        struct Field {
            std::string Name;
            QualifiedType *Type;
        };
    protected:
        std::string Name;
        std::vector<Field *> FieldList;
    public:
        constexpr explicit StructType(const std::string_view Name) noexcept
        : Type(TypeKind::Structure), Name(Name) {}

        constexpr explicit StructType(std::string &&Name) noexcept
        : Type(TypeKind::Structure), Name(std::move(Name)) {}

        [[nodiscard]] constexpr static auto IsOfKind(const Type &Ty) noexcept {
            return Ty.getKind() == TyKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Type *const Type) noexcept {
            return IsOfKind(*Type);
        }

        [[nodiscard]]
        constexpr std::string_view getName() const noexcept override {
            return Name;
        }

        [[nodiscard]] constexpr auto &getFieldList() const noexcept {
            return FieldList;
        }

        [[nodiscard]] constexpr auto &getFieldListRef() noexcept {
            return FieldList;
        }
    };
}
