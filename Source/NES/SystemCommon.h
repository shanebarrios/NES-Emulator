#pragma once

inline constexpr usize WRAM_SIZE = 0x800;
inline constexpr usize SYSTEM_PALETTE_LENGTH = 0x40;

using WRAM = Memory<WRAM_SIZE>;