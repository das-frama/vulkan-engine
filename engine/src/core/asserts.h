#pragma once

#include "defines.h"

// Disable assertions by commenting out the bellow line.
#define ASSERTIONS_ENABLED

#ifdef ASSERTIONS_ENABLED
#if _MS_VER
#include <intrin.h>
#define debug_break() __debugbreak()
#else
#define debug_break() __builtin_trap()
#endif

API void report_assert_failure(const char *expr, const char *msg, const char *file, i32 line);

#define ASSERT(expr)                                              \
    {                                                             \
        if (expr) {                                               \
        } else {                                                  \
            report_assert_failure(#expr, "", __FILE__, __LINE__); \
            debug_break();                                        \
        }                                                         \
    }

#define ASSERT_MSG(expr, message)                                      \
    {                                                                  \
        if (expr) {                                                    \
        } else {                                                       \
            report_assert_failure(#expr, message, __FILE__, __LINE__); \
            debug_break();                                             \
        }                                                              \
    }

#ifdef _DEBUG
#define ASSERT_DEBUG(expr)                                        \
    {                                                             \
        if (expr) {                                               \
        } else {                                                  \
            report_assert_failure(#expr, "", __FILE__, __LINE__); \
            debug_break();                                        \
        }                                                         \
    }
#else
#define ASSERT_DEBUG(expr)
#endif

#else
#define ASSERT(expr)
#define ASSERT_MSG(expr, message)
#define ASSERT_DEBUG(expr)

#endif