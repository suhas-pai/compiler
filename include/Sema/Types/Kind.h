/*
 * Sema/Types/Kind.h
 * © suhas pai
 */

#pragma once

namespace Sema {
    enum class TypeKind {
        Builtin,
        Pointer,
        Structure,
        Union,
        FunctionPrototype,
        Enum
    };
}
