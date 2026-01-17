#pragma once

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#ifdef _MSC_VER
#include <intrin.h>
#define DEBUG_BREAK() __debugbreak()
#else
#include <signal.h>
#define DEBUG_BREAK() raise(SIGTRAP)
#endif

#ifndef ENABLE_ASSERTS
#ifdef _DEBUG
#define ENABLE_ASSERTS
#endif
#endif

#ifdef ENABLE_ASSERTS
#define ASSERT(expr)                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            fprintf(stderr, "Assertion failed: %s\n%s:%d\n", #expr, __FILE__, __LINE__);                               \
            DEBUG_BREAK();                                                                                             \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (0)

#define ASSERT_MSG(expr, msg)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            fputs(msg, stderr);                                                                                        \
            DEBUG_BREAK();                                                                                             \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (0)
#else
#define ASSERT(expr)
#define ASSERT_MSG(expr, msg)
#endif

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using usize = size_t;
using isize = ptrdiff_t;

using bool8 = uint8_t;
using bool32 = uint32_t;

template <typename T, size_t N> using Array = std::array<T, N>;

template <typename T, size_t M, size_t N> using Array2D = std::array<std::array<T, M>, N>;

template <size_t N> using Memory = Array<u8, N>;
