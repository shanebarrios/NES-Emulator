#pragma once

#include <array>
#include <span>
#include <string>

enum class AddrMode;
enum class InstrType;

namespace DebugUtils
{
	std::array<char, 2> ByteToChars(uint8_t byte);

	const char* InstrTypeToStr(InstrType instrType);

	void AddrModeToStr(AddrMode mode, uint16_t operand, uint16_t effectiveAddr, uint8_t val, char* buf, size_t bufLen);
}