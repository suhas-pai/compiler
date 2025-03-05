/*
 * Sema/Types/PointerQualifiers.h
 * © suhas pai
 */

#pragma once

namespace Sema {
    struct PointerBaseTypeQualifiers {
        bool isMutable : 1 = false;
        bool isVolatile : 1 = false;
        bool isUnaliased : 1 = false;
    };
}
