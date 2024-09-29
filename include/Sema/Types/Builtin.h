/*
 * /Types/Builtin.h
 * © suhas pai
 */

#pragma once

#include <string>
#include "Type.h"

namespace Sema {
    struct BuiltinType : public Type {
    public:
        constexpr static auto TyKind = TypeKind::Builtin;
    protected:
        std::string Name;

        constexpr explicit BuiltinType(std::string_view Name) noexcept
        : Type(TypeKind::Builtin), Name(Name) {}
    public:
        [[nodiscard]] static inline auto IsOfKind(const Type &Type) noexcept {
            return Type.getKind() == TyKind;
        }

        [[nodiscard]]
        static inline auto classof(const Type *const Type) noexcept {
            return IsOfKind(*Type);
        }

        [[nodiscard]] static auto u16() noexcept -> const BuiltinType & {
            static auto Result = BuiltinType("u16");
            return Result;
        }

        [[nodiscard]] static auto u32() noexcept -> const BuiltinType & {
            static auto Result = BuiltinType("u32");
            return Result;
        }

        [[nodiscard]] static auto u64() noexcept -> const BuiltinType & {
            static auto Result = BuiltinType("u64");
            return Result;
        }

        [[nodiscard]] static auto s16() noexcept -> const BuiltinType & {
            static auto Result = BuiltinType("s16");
            return Result;
        }

        [[nodiscard]] static auto s32() noexcept -> const BuiltinType & {
            static auto Result = BuiltinType("s32");
            return Result;
        }

        [[nodiscard]] static auto s64() noexcept -> const BuiltinType & {
            static auto Result = BuiltinType("s64");
            return Result;
        }

        [[nodiscard]] static auto f32() noexcept -> const BuiltinType & {
            static auto Result = BuiltinType("f32");
            return Result;
        }

        [[nodiscard]] static auto f64() noexcept -> const BuiltinType & {
            static auto Result = BuiltinType("float64");
            return Result;
        }

        [[nodiscard]] static auto boolType() noexcept -> const BuiltinType & {
            static auto Result = BuiltinType("bool");
            return Result;
        }

        [[nodiscard]]
        constexpr std::string_view getName() const noexcept override {
            return Name;
        }

        [[nodiscard]]
        constexpr static auto forName(const std::string_view Name) noexcept
            -> const BuiltinType *
        {
            if (Name == "u16") {
                return &u16();
            } else if (Name == "u32") {
                return &u32();
            } else if (Name == "u64") {
                return &u64();
            } else if (Name == "s16") {
                return &s16();
            } else if (Name == "s32") {
                return &s32();
            } else if (Name == "s64") {
                return &s64();
            } else if (Name == "f32") {
                return &f32();
            } else if (Name == "f64") {
                return &f64();
            } else if (Name == "bool") {
                return &boolType();
            }

            return nullptr;
        }
    };
};

