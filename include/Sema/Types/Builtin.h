/*
 * Sema/Types/Builtin.h
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
        [[nodiscard]] static inline auto IsOfKind(const Type &Ty) noexcept {
            return Ty.getKind() == TyKind;
        }

        [[nodiscard]]
        static inline auto classof(const Type *const Type) noexcept {
            return IsOfKind(*Type);
        }

        [[nodiscard]] static auto voidType() noexcept -> BuiltinType & {
            static auto Result = BuiltinType("void");
            return Result;
        }

        [[nodiscard]] static auto u8() noexcept -> BuiltinType & {
            static auto Result = BuiltinType("u8");
            return Result;
        }

        [[nodiscard]] static auto u16() noexcept -> BuiltinType & {
            static auto Result = BuiltinType("u16");
            return Result;
        }

        [[nodiscard]] static auto u32() noexcept -> BuiltinType & {
            static auto Result = BuiltinType("u32");
            return Result;
        }

        [[nodiscard]] static auto u64() noexcept -> BuiltinType & {
            static auto Result = BuiltinType("u64");
            return Result;
        }

        [[nodiscard]] static auto s8() noexcept -> BuiltinType & {
            static auto Result = BuiltinType("s8");
            return Result;
        }

        [[nodiscard]] static auto s16() noexcept -> BuiltinType & {
            static auto Result = BuiltinType("s16");
            return Result;
        }

        [[nodiscard]] static auto s32() noexcept -> BuiltinType & {
            static auto Result = BuiltinType("s32");
            return Result;
        }

        [[nodiscard]] static auto s64() noexcept -> BuiltinType & {
            static auto Result = BuiltinType("s64");
            return Result;
        }

        [[nodiscard]] static auto f32() noexcept -> BuiltinType & {
            static auto Result = BuiltinType("f32");
            return Result;
        }

        [[nodiscard]] static auto f64() noexcept -> BuiltinType & {
            static auto Result = BuiltinType("f64");
            return Result;
        }

        [[nodiscard]] static auto boolType() noexcept -> BuiltinType & {
            static auto Result = BuiltinType("bool");
            return Result;
        }

        [[nodiscard]]
        constexpr std::string_view getName() const noexcept override {
            return Name;
        }

        [[nodiscard]]
        constexpr static auto forName(const std::string_view Name) noexcept
            -> BuiltinType *
        {
            if (Name == "void") {
                return &voidType();
            }

            if (Name == "u8") {
                return &u8();
            }

            if (Name == "u16") {
                return &u16();
            }

            if (Name == "u32") {
                return &u32();
            }

            if (Name == "u64") {
                return &u64();
            }

            if (Name == "s8") {
                return &s8();
            }

            if (Name == "s16") {
                return &s16();
            }

            if (Name == "s32") {
                return &s32();
            }

            if (Name == "s64") {
                return &s64();
            }

            if (Name == "f32") {
                return &f32();
            }

            if (Name == "f64") {
                return &f64();
            }

            if (Name == "bool") {
                return &boolType();
            }

            return nullptr;
        }
    };
};

