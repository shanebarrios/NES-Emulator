#pragma once

#include <cstdlib>
#include <iostream>

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