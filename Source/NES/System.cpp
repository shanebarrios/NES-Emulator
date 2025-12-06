#include "System.h"

void System::Init()
{
	m_Bus.Attach(&m_CPU, &m_Cartridge, m_RAM.data());
	m_CPU.Init(&m_Bus);
}

void System::LoadROM(const std::filesystem::path& path)
{
	m_Cartridge.LoadROM(path);
	m_CPU.Reset();
}

void System::PerformCycle()
{
	m_CPU.PerformCycle();
}