#include "System.h"

#include <fstream>
#include <stdexcept>

System::System() 
{
	m_Mapper = std::make_unique<NROM>();

	m_Mapper->Attach(&m_Cartridge);
	m_Ppu.Attach(&m_Cpu, m_Mapper.get(), m_SystemPalette.data());
	m_CpuBus.Attach(m_Mapper.get(), &m_Ppu, m_SystemRam.data());
	m_Cpu.Attach(&m_CpuBus);
}

void System::LoadROM(const std::filesystem::path& path)
{
	m_Cartridge.LoadFromFile(path);
}

void System::LoadPalette(const std::filesystem::path& path, u32 num)
{
	std::ifstream inf{ path, std::ios::binary };

	if (!inf)
		throw std::runtime_error{ "Failed to load system palette file" };

	constexpr usize READ_PALETTE_SIZE = SYSTEM_PALETTE_LENGTH * 3;
	Array<char, READ_PALETTE_SIZE> buf;

	inf.seekg(num * (READ_PALETTE_SIZE), std::ios::beg);
	if (!inf)
		throw std::runtime_error{ "System palette offset out of bounds" };

	inf.read(buf.data(), buf.size());

	if (inf.gcount() != buf.size())
		throw std::runtime_error{ "Invalid system palette file" };

	for (usize i = 0; i < m_SystemPalette.size(); i++)
	{
		m_SystemPalette[i].r = buf[i * 3];
		m_SystemPalette[i].g = buf[i * 3 + 1];
		m_SystemPalette[i].b = buf[i * 3 + 2];
	}
}

void System::Reset()
{
	m_Cpu.Reset();
	m_Ppu.Reset();
}

void System::Update()
{
	m_Cpu.PerformCycle();
	m_Ppu.PerformCycle();
	m_Ppu.PerformCycle();
	m_Ppu.PerformCycle();
}