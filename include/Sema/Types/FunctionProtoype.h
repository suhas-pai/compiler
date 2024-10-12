/*
 * Sema/Types/FunctionPrototype.h
 */

#pragma once

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
        FunctionPrototype(
            QualifiedType *const ReturnType,
            const std::vector<QualifiedType *> &ParamList) noexcept
        : Type(ObjKind), ReturnType(ReturnType), ParamList(ParamList) {}

        constexpr explicit
        FunctionPrototype(QualifiedType *const ReturnType,
                          std::vector<QualifiedType *> &&ParamList) noexcept
        : Type(ObjKind), ReturnType(ReturnType),
          ParamList(std::move(ParamList)) {}

        [[nodiscard]] static inline auto IsOfKind(const Type &Ty) noexcept {
            return Ty.getKind() == ObjKind;
        }

        [[nodiscard]]
        static inline auto classof(const Type *const Type) noexcept {
            return IsOfKind(*Type);
        }

        [[nodiscard]] constexpr auto &getReturnType() const noexcept {
            return *ReturnType;
        }

        [[nodiscard]] constexpr auto &getParamList() const noexcept {
            return ParamList;
        }

        [[nodiscard]] constexpr auto &getParamListRef() noexcept {
            return ParamList;
        }
    };
}
