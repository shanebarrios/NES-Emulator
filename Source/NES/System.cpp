#include "System.h"

System::System() 
{
	m_SystemRam = std::make_unique<u8[]>(SYSTEM_RAM_SIZE);
	m_Cartridge = std::make_unique<Cartridge>();
	m_Mapper = std::make_unique<NROM>();
	m_Ppu = std::make_unique<PPU>();
	m_CpuBus = std::make_unique<CPUBus>();
	m_Cpu = std::make_unique<CPU>();

	m_Mapper->Attach(m_Cartridge.get());
	m_Ppu->Attach(m_Mapper.get());
	m_CpuBus->Attach(m_Mapper.get(), m_Ppu.get(), m_SystemRam.get());
	m_Cpu->Attach(m_CpuBus.get());
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