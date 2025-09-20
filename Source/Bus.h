#pragma once

#include <array>
#include <cstdint>
#include <filesystem>

class Bus
{
public:
	uint8_t Read(uint16_t addr) const;
	void Write(uint16_t addr, uint8_t val);

	// TODO: create cartridge class
	void LoadROM(const std::filesystem::path& path);

private:
	std::array<uint8_t, 0x800> m_InternalRam{};

	// TOOD: move this to cartridge
	std::array<uint8_t, 0x8000> m_CartridgeROM{};
};