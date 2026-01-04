#include "Cartridge.h"

#include <fstream>
#include <cstring>

CartridgeLoadResult Cartridge::LoadFromFile(const std::filesystem::path& path)
{
	std::ifstream inf{ path, std::ios::binary };

	if (!inf)
	{
		return CartridgeLoadResult::FileNotFound;
	}

	const char expectedHeader[4] = { 'N', 'E', 'S', '\x1A' };

	u8 header[16];

	inf.read(reinterpret_cast<char*>(header), 16);

	if (inf.eof() || std::strncmp(reinterpret_cast<char*>(header), expectedHeader, 4) != 0)
	{
		return CartridgeLoadResult::InvalidHeader;
	}

	m_PrgRomSize = header[4] * 0x4000;
	if (m_PrgRomSize == 0)
	{
		return CartridgeLoadResult::InvalidPrgRomSize;
	}
	m_PrgRom = std::make_unique<u8[]>(m_PrgRomSize);

	m_ChrSize = header[5] * 0x2000;
	if (m_ChrSize == 0)
	{
		m_ChrSize = 0x2000;
		m_ChrRam = std::make_unique<u8[]>(0x2000);
	}
	else
	{
		m_ChrRom = std::make_unique<u8[]>(m_ChrSize);
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
		m_PrgRamSize = 0x8000u;
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
			return CartridgeLoadResult::MissingData;
	}

	inf.read(reinterpret_cast<char*>(m_PrgRom.get()), m_PrgRomSize);

	if (m_ChrRom)
	{
		inf.read(reinterpret_cast<char*>(m_ChrRom.get()), m_ChrSize);
	}

	if (inf.eof())
	{
		return CartridgeLoadResult::MissingData;
	}

	return CartridgeLoadResult::Success;
}

u8 Cartridge::ReadPrgRom(usize offset) const
{
	return m_PrgRom[offset & (m_PrgRomSize - 1)];
}

u8 Cartridge::ReadChr(usize offset) const
{
	offset &= m_ChrSize - 1;

	if (m_ChrRom)
	{
		return m_ChrRom[offset];
	}
	else
	{
		return m_ChrRam[offset];
	}
}

std::optional<u8> Cartridge::ReadPrgRam(usize offset) const
{
	if (!m_PrgRam)
	{
		return std::nullopt;
	}

	return m_PrgRam[offset & (m_PrgRamSize - 1)];
}

void Cartridge::WritePrgRam(usize offset, u8 data)
{
	if (!m_PrgRam)
	{
		return;
	}

	m_PrgRam[offset & (m_PrgRamSize - 1)] = data;
}

void Cartridge::WriteChr(usize offset, u8 data)
{
	offset &= m_ChrSize - 1;

	if (m_ChrRam)
	{
		m_ChrRam[offset] = data;
	}
}
