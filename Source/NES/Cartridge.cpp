#include "Cartridge.h"

#include <fstream>
#include <cstring>
#include <stdexcept>

void Cartridge::LoadFromFile(const std::filesystem::path& path)
{
	std::ifstream inf{ path, std::ios::binary };

	if (!inf)
	{
		throw std::runtime_error{ "Failed to open rom file" };
	}

	const char expectedHeader[4] = { 'N', 'E', 'S', '\x1A' };

	u8 header[16];

	inf.read(reinterpret_cast<char*>(header), 16);

	if (inf.eof() || std::strncmp(reinterpret_cast<char*>(header), expectedHeader, 4) != 0)
	{
		throw std::runtime_error{ "Invalid rom header" };
	}

	m_PrgRomSize = header[4] * 0x4000;
	if (m_PrgRomSize == 0)
	{
		throw std::runtime_error{ "Invalid PRG_ROM size" };
	}
	m_PrgRom = std::make_unique<u8[]>(m_PrgRomSize);

	m_ChrRomSize = header[5] * 0x2000;
	if (m_ChrRomSize == 0)
	{
		m_ChrRam = std::make_unique<u8[]>(0x2000);
	}
	else
	{
		m_ChrRom = std::make_unique<u8[]>(m_ChrRomSize);
	}

	if (header[6] & (1 << 0))
	{
		m_MirrorMode = MirrorMode::Vertical;
	}
	else
	{
		m_MirrorMode = MirrorMode::Horizontal;
	}

	m_HasPrgRam = header[6] & (1 << 1);
	m_HasTrainer = header[6] & (1 << 2);
	if (header[6] & (1 << 3))
	{
		m_MirrorMode = MirrorMode::FourScreen;
	}
	m_MapperNumber = (header[6] >> 4) | (header[7] & 0xF0);

	m_PrgRamSize = header[8] * 0x2000;
	if (m_HasPrgRam && m_PrgRamSize == 0)
	{
		m_PrgRamSize = 0x2000u;
	}
	if (m_HasPrgRam)
	{
		m_PrgRam = std::make_unique<u8[]>(m_PrgRamSize);
	}

	if (m_HasTrainer)
	{
		m_Trainer = std::make_unique<u8[]>(0x200);
		inf.read(reinterpret_cast<char*>(m_Trainer.get()), 0x200);
		if (inf.eof())
			throw std::runtime_error{ "Incomplete trainer / missing ROM data" };
	}

	inf.read(reinterpret_cast<char*>(m_PrgRom.get()), m_PrgRomSize);

	if (m_ChrRomSize > 0)
	{
		inf.read(reinterpret_cast<char*>(m_ChrRom.get()), m_ChrRomSize);
	}

	if (inf.eof())
	{
		throw std::runtime_error{ "Missing ROM data" };
	}	
}

u8 Cartridge::ReadPrgRom(u16 offset) const
{
	if (offset >= m_PrgRomSize)
	{
		return 0;
	}

	return m_PrgRom[offset];
}

u8 Cartridge::ReadChrRom(u16 offset) const
{
	if (offset >= m_ChrRomSize)
	{
		return 0;
	}

	return m_ChrRom[offset];
}

u8 Cartridge::ReadPrgRam(u16 offset) const
{
	if (offset >= m_PrgRamSize)
	{
		return 0;
	}

	return m_PrgRam[offset];
}

u8 Cartridge::ReadChrRam(u16 offset) const
{
	if (!m_ChrRam || offset >= 0x2000)
	{
		return 0;
	}

	return m_ChrRam[offset];
}

void Cartridge::WritePrgRam(u16 offset, u8 data)
{
	if (offset >= m_PrgRamSize)
	{
		return;
	}

	m_PrgRam[offset] = data;
}

void Cartridge::WriteChrRam(u16 offset, u8 data)
{
	if (!m_ChrRam || offset >= 0x2000)
	{
		return;
	}

	m_ChrRam[offset] = data;
}