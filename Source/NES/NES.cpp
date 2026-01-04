#include "NES.h"

#include "../Core/Logger.h"

NES::NES() 
{
	
}

bool NES::LoadROM(const std::filesystem::path& path)
{
	const std::string pathStr = path.string();
	const std::string fileStr = path.filename().string();

	if (m_Cartridge.LoadFromFile(path) != CartridgeLoadResult::Success)
	{
		LOG_ERROR("Failed to load ROM at %s", pathStr.c_str());
		return false;
	}

	switch (m_Cartridge.GetMapperNumber())
	{
	case 0:
		m_Mapper = std::make_unique<NROM>();
		break;
	case 1:
		m_Mapper = std::make_unique<MMC1>();
		break;
	case 3:
		m_Mapper = std::make_unique<CNROM>();
		break;
	default:
		LOG_ERROR("Cartridge at %s has unsupported mapper", pathStr.c_str());
		return false;
	}

	m_Mapper->Init(&m_Cartridge, &m_Cpu);
	m_Ppu.Attach(&m_Cpu, m_Mapper.get());
	m_CpuBus.Attach(m_Mapper.get(), &m_Ppu, m_Wram.data(), &m_Controller);
	m_Cpu.Attach(&m_CpuBus);

	auto toKB = [](usize sizeBytes){ return sizeBytes >> 10; };

	LOG_INFO("Loaded cartridge %s, PRG_ROM=%uKB, PRG_RAM=%uKB, CHR=%uKB, Mapper=%u",
			 fileStr.c_str(),
			 toKB(m_Cartridge.GetPrgRomSize()),
			 toKB(m_Cartridge.GetPrgRamSize()),
			 toKB(m_Cartridge.GetChrSize()),
			 m_Cartridge.GetMapperNumber());
	
	return true;
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
