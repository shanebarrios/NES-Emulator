#include "Bus.h"

#include "Common.h"

#include <fstream>
#include <cstring>
#include <stdexcept>

uint8_t Bus::Read(uint16_t addr) const
{
	if (addr < 0x2000)
	{
		return m_InternalRam[addr % 0x800];
	}
	else if (addr < 0x4000)
	{
		// PPU, do nothing for now
	}
	else if (addr < 0x4020)
	{
		// APU and I/O functionality, normally disabled
	}
	else if (addr < 0x8000)
	{

	}
	else
	{
		return m_CartridgeROM[addr % 0x8000];
	}
	ASSERT_MSG(false, "Unsupported memory read!");
	return 0;
}

void Bus::Write(uint16_t addr, uint8_t val)
{
	if (addr < 0x2000)
	{
		m_InternalRam[addr % 0x800] = val;
	}
	else if (addr < 0x4000)
	{
		// PPU, do nothing for now
	}
	else if (addr < 0x4020)
	{
		// APU and I/O functionality, normally disabled
	}
	else if (addr < 0x8000)
	{
		// Unmapped, do nothing for now
	}
	else
	{
		m_CartridgeROM[addr % 0x8000] = val;
	}
}

void Bus::LoadROM(const std::filesystem::path& path)
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
	inf.read(reinterpret_cast<char*>(m_CartridgeROM.data()), romSize16KB * 0x4000);

	if (romSize16KB == 1)
	{
		std::memcpy(m_CartridgeROM.data() + 0x4000, m_CartridgeROM.data(), 0x4000);
	}
}