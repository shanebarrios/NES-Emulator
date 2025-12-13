#pragma once

inline constexpr usize SYSTEM_RAM_SIZE = 0x800;
inline constexpr usize SYSTEM_PALETTE_LENGTH = 0x40;

struct RGB8
{
	u8 r;
	u8 g;
	u8 b;
};

using SystemRam = Memory<SYSTEM_RAM_SIZE>;
using SystemPalette = Array<RGB8, SYSTEM_PALETTE_LENGTH>;