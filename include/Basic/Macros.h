/*
 * Basic/Macros.h
 */

#pragma once

#define SV_FMT "%.*s"
#define SV_FMT_ARG(sv) static_cast<int>((sv).length()), (sv).data()
