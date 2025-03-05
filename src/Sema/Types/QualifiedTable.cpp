/*
 * Sema/Types/QualifiedTable.cpp
 * © suhas pai
 */

#include "Sema/Types/QualifiedTable.h"

namespace Sema {
    auto
    QualifiedTypeTable::getQualifiedTypeOrInsert(
        Type *const UnderlyingTy,
        const TypeQualifiers Qual) noexcept
            -> QualifiedType &
    {
        if (const auto It = Table.find(UnderlyingTy); It != Table.end()) {
            return It->second;
        }

        const auto QualTy = QualifiedType(UnderlyingTy, Qual);
        return Table.emplace(UnderlyingTy, QualTy).first->second;
    }
}
