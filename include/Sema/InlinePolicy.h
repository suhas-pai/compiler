/*
 * Sema/InlinePolicy.h
 * © suhas pai
 */

#pragma once

namespace Sema {
    enum class InlinePolicy {
        None,
        Default,
        DontInline,
        AlwaysInline,
        NeverInline,
    };
}
