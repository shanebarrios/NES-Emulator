#pragma once

#include "../Core/Common.h"
#include <array>

enum class AddrMode;
enum class InstrType;
enum class Op;

namespace DebugUtils
{
	std::array<char, 2> ByteToChars(u8 byte);

	const char* OpToStr(Op op);

	//void AddrModeToStr(AddrMode mode, u16 operand, u16 effectiveAddr, u8 val, char* buf, usize bufLen);
}