/*
 * Sema/Types/FunctionPrototype.h
 * © suhas pai
 */

#pragma once

#include <span>
#include <vector>

#include "Sema/Types/Qualified.h"

namespace Sema {
    struct FunctionPrototype : public Type {
    public:
        constexpr static auto ObjKind = TypeKind::FunctionPrototype;
    protected:
        QualifiedType *ReturnType;
        std::vector<QualifiedType *> ParamList;
    public:
        constexpr explicit
        FunctionPrototype(QualifiedType *const ReturnType,
                          const std::span<QualifiedType *> ParamList) noexcept
        : Type(ObjKind), ReturnType(ReturnType),
          ParamList(std::vector(ParamList.begin(), ParamList.end())) {}

        constexpr explicit
        FunctionPrototype(QualifiedType *const ReturnType,
                          std::vector<QualifiedType *> &&ParamList) noexcept
        : Type(ObjKind), ReturnType(ReturnType),
          ParamList(std::move(ParamList)) {}

        [[nodiscard]] constexpr static auto IsOfKind(const Type &Ty) noexcept {
            return Ty.getKind() == ObjKind;
        }

        [[nodiscard]]
        constexpr static auto classof(const Type *const Type) noexcept {
            return IsOfKind(*Type);
        }

        [[nodiscard]] constexpr auto &getReturnType() const noexcept {
            return *this->ReturnType;
        }

        [[nodiscard]] constexpr auto getParamList() const noexcept {
            return std::span(this->ParamList);
        }

        [[nodiscard]] constexpr auto &getParamListRef() noexcept {
            return this->ParamList;
        }
    };
}
