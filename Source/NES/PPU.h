#pragma once

#include <array>
#include "Common.h"

class Cartridge;

class PPU
{
public:
	u8 ReportStatus() const;

private:
	u8 Read(u16 addr) const;

	void Write(u16 addr, u8 val);

	std::array<u8, 0x800> m_VRAM;
	std::array<u8, 0x100> m_SPRAM;
	std::array<u8, 0x20> m_Palette;

	Cartridge* m_Cartridge = nullptr;

	bool m_NMIEnabled = true;
	bool m_8x8Sprites = true;
	
};