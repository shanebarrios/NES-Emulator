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
	case 3:
		m_Mapper = std::make_unique<CNROM>();
		break;
	default:
		throw std::runtime_error{ "Cartridge has unsupported mapper" };
	}

	m_Mapper->Init(&m_Cartridge, &m_Cpu);
	m_Ppu.Attach(&m_Cpu, m_Mapper.get());
	m_CpuBus.Attach(m_Mapper.get(), &m_Ppu, m_Wram.data(), &m_Controller);
	m_Cpu.Attach(&m_CpuBus);
}

void NES::Reset()
{
	m_Cpu.Reset();
	m_Ppu.Reset();
}

void NES::StepFrame()
{
	while (!m_Ppu.FramebufferReady())
	{
		Update();
	}
	m_Ppu.ClearFramebufferReady();
}

void NES::Update()
{
	m_Cpu.PerformCycle();
	m_Ppu.PerformCycle();
	m_Ppu.PerformCycle();
	m_Ppu.PerformCycle();
}

const u8* NES::GetFramebuffer() const
{
	return m_Ppu.GetFramebuffer();
}

void NES::SetButtonsState(u8 state)
{
	m_Controller.SetButtonsState(state);
}