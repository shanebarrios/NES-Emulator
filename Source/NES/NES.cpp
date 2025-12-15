#include "NES.h"

#include <stdexcept>

NES::NES() 
{
	
}

void NES::LoadROM(const std::filesystem::path& path)
{
	m_Cartridge.LoadFromFile(path);

	switch (m_Cartridge.GetMapperNumber())
	{
	case 0:
		m_Mapper = std::make_unique<NROM>();
		break;
	default:
		throw std::runtime_error{ "Cartridge has unsupported mapper" };
	}

	m_Mapper->Attach(&m_Cartridge);
	m_Ppu.Attach(&m_Cpu, m_Mapper.get());
	m_CpuBus.Attach(m_Mapper.get(), &m_Ppu, m_SystemRam.data());
	m_Cpu.Attach(&m_CpuBus);
}

void NES::Reset()
{
	m_Cpu.Reset();
	m_Ppu.Reset();
}

void NES::Update()
{
	m_Cpu.PerformCycle();
	m_Ppu.PerformCycle();
	m_Ppu.PerformCycle();
	m_Ppu.PerformCycle();
}