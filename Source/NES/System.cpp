#include "System.h"

System::System() 
{
	m_SystemRam = std::make_unique<u8[]>(SYSTEM_RAM_SIZE);
	m_Cartridge = std::make_unique<Cartridge>();
	m_Mmc = std::make_unique<Mapper0>(m_Cartridge.get());
	m_CpuBus = std::make_unique<CPUBus>(m_Mmc.get(), m_SystemRam.get());
	m_Cpu = std::make_unique<CPU>(m_CpuBus.get());
}

System::~System()
{
	
}

void System::LoadROM(const std::filesystem::path& path)
{
	m_Cartridge->LoadFromFile(path);
	m_Cpu->Reset();
}

void System::PerformCycle()
{
	m_Cpu->PerformCycle();
}