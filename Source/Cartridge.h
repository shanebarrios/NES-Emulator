#pragma once

#include "Common.h"
#include <filesystem>
#include <array>

class Cartridge
{
public:
	Cartridge() = default;

	void LoadROM(const std::filesystem::path& path);

	u8 ReadRAM(u16 addr) const;

	u8 ReadROM(u16 addr) const;

private:
	std::array<u8, 0x2000> m_RAM{};
	std::array<u8, 0x8000> m_ROM{};
};