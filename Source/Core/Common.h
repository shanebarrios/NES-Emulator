#pragma once

#include <cstdlib>
#include <iostream>
#include <cstdint>
#include <array>

#define ASSERT(cond) \
	do { \
		if(!(cond)){ \
			std::cerr << "Assertion failed: " << #cond << '\n'; \
			std::abort(); \
		}\
	} while (0)

#define ASSERT_MSG(cond, msg) \
	do { \
		if (!cond) { \
			std::cerr << msg << '\n'; \
			std::abort(); \
		} \
	} while (0)

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

template <typename T, size_t N>
using Array = std::array<T, N>;

template <typename T, size_t M, size_t N>
using Array2D = std::array<std::array<T, M>, N>;

template <size_t N>
using Memory = Array<u8, N>;