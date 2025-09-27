#pragma once

#include <array>
#include <span>

enum class InstrType;

namespace DebugUtils
{
	std::array<char, 2> ByteToChars(uint8_t byte);

	const char* InstrTypeToStr(InstrType instrType);
}