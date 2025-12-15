#pragma once

inline constexpr usize SYSTEM_RAM_SIZE = 0x800;
inline constexpr usize SYSTEM_PALETTE_LENGTH = 0x40;

using SystemRam = Memory<SYSTEM_RAM_SIZE>;