#include "Cartridge.h"

#include <fstream>
#include <cstring>
#include <stdexcept>

void Cartridge::LoadROM(const std::filesystem::path& path)
{
	std::ifstream inf{ path, std::ios::binary };

	if (!inf)
	{
		throw std::runtime_error{ "Failed to open rom file" };
	}

	const char expectedHeader[4] = { 'N', 'E', 'S', '\x1A' };
	char header[4];

	inf.read(header, 4);

	if (std::strncmp(header, expectedHeader, 4) != 0)
		throw std::runtime_error{ "Invalid rom header" };

	uint8_t romSize16KB;
	inf.read(reinterpret_cast<char*>(&romSize16KB), 1);

	if (romSize16KB != 1 && romSize16KB != 2)
	{
		throw std::runtime_error{ "Invalid rom size" };
	}

	inf.seekg(16, std::ios::beg);
	inf.read(reinterpret_cast<char*>(m_ROM.data()), romSize16KB * 0x4000);

	if (romSize16KB == 1)
	{
		std::memcpy(m_ROM.data() + 0x4000, m_ROM.data(), 0x4000);
	}
}

u8 Cartridge::ReadROM(u16 addr) const
{
	return m_ROM[addr];
}

u8 Cartridge::ReadRAM(u16 addr) const
{
	return m_RAM[addr];
}