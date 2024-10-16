/*
 * Basic/Macros.h
 */

#pragma once

#define SV_FMT "%.*s"
#define SV_FMT_ARG(sv) static_cast<int>((sv).length()), (sv).data()

#define sizeof_bits(type) (sizeof(type) * 8)

#if !defined(__printflike)
    #define __printflike(fmtarg, firstvararg) \
        __attribute__((__format__ (__printf__, fmtarg, firstvararg)))
#endif /* !defined(__printflike) */
