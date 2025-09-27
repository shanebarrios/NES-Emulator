#pragma once

#include <filesystem>
#include <cstdint>
#include <array>

class Cartridge
{
public:
	Cartridge() = default;

	void LoadROM(const std::filesystem::path& path);

	uint8_t ReadRAM(uint16_t addr) const;

	uint8_t ReadROM(uint16_t addr) const;

private:
	std::array<uint8_t, 0x2000> m_RAM{};
	std::array<uint8_t, 0x8000> m_ROM{};
};