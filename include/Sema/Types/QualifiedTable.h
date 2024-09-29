/*
 * Sema/Types/QualifiedTable.h
 * © suhas pai
 */

#pragma once
#include <unordered_map>

#include "Sema/Types/Qualified.h"
#include "Sema/Types/Qualifiers.h"

namespace Sema {
    // Create a central table for AST::QualifiedType. This allows canonical
    // types to be looked up in O(1) rather than O(n).

    class QualifiedTypeTable {
    protected:
        std::unordered_map<Type *, QualifiedType> Table;
    public:
        explicit QualifiedTypeTable() = default;

        QualifiedType &
        getQualifiedTypeOrInsert(Type *UnderlyingTy,
                                 TypeQualifiers Qual) noexcept;
    };
}