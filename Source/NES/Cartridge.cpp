#include "Cartridge.h"

#include <fstream>
#include <cstring>
#include <stdexcept>

Cartridge::~Cartridge()
{
	delete[] m_ChrRomBanks;
	delete[] m_PrgRomBanks;
	delete[] m_RamBanks;
	delete[] m_Trainer;
}

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
		throw std::runtime_error{ "Invalid rom header" };

	m_NumPrgRomBanks = header[4];

	if (m_NumPrgRomBanks == 0)
		throw std::runtime_error{ "Invalid number of PRG_ROM banks" };

	m_NumChrRomBanks = header[5];

	if (m_NumChrRomBanks == 0)
		throw std::runtime_error{ "Invalid number of CHR_ROM banks" };

	m_PrgRomBanks = new PrgRomBank[m_NumPrgRomBanks];
	m_ChrRomBanks = new ChrRomBank[m_NumChrRomBanks];

	if (header[6] & (1 << 0))
		m_MirrorMode = MirrorMode::Horizontal;
	else
		m_MirrorMode = MirrorMode::Vertical;

	m_HasRam = header[6] & (1 << 1);
	m_HasTrainer = header[6] & (1 << 2);

	if (header[6] & (1 << 3))
		m_MirrorMode = MirrorMode::FourScreen;

	m_MapperNumber = (header[6] >> 4) | (header[7] & 0xF0);

	m_NumRamBanks = header[8];
	if (m_HasRam && m_NumRamBanks == 0)
		m_NumRamBanks = 1;

	if (m_HasRam)
		m_RamBanks = new RamBank[m_NumRamBanks];

	if (m_HasTrainer)
	{
		m_Trainer = new u8[TRAINER_SIZE];
		inf.read(reinterpret_cast<char*>(m_Trainer), TRAINER_SIZE);
		if (inf.eof())
			throw std::runtime_error{ "Incomplete trainer / missing ROM data" };
	}

	inf.read(reinterpret_cast<char*>(m_PrgRomBanks), m_NumPrgRomBanks * PRG_ROM_BANK_SIZE);
	inf.read(reinterpret_cast<char*>(m_ChrRomBanks), m_NumChrRomBanks * CHR_ROM_BANK_SIZE);

	if (inf.eof())
		throw std::runtime_error{ "Missing ROM data" };
	
}

u8 Cartridge::ReadPrgRom(u8 bank, u16 offset) const
{
	ASSERT(bank < m_NumPrgRomBanks);

	return m_PrgRomBanks[bank][offset];
}

u8 Cartridge::ReadChrRom(u8 bank, u16 offset) const
{
	ASSERT(bank < m_NumChrRomBanks);

	return m_ChrRomBanks[bank][offset];
}

u8 Cartridge::ReadRam(u8 bank, u16 offset) const
{
	ASSERT(bank < m_NumRamBanks);

	return m_RamBanks[bank][offset];
}

void Cartridge::WriteRam(u8 bank, u16 offset, u8 data)
{
	ASSERT(bank < m_NumRamBanks);

	m_RamBanks[bank][offset] = data;
}