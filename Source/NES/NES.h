#pragma once

#include "../Core/Common.h"
#include "CPU.h"
#include "CPUBus.h"
#include "Cartridge.h"
#include "SystemCommon.h"
#include "PPU.h"
#include "HardwareController.h"

#include <filesystem>
#include <memory>

class NES
{
public:
	static constexpr double MASTER_CLOCK_PERIOD = 1.0 / (21.477272e6);
	// Period between every CPU cycle
	static constexpr double UPDATE_PERIOD = MASTER_CLOCK_PERIOD * 12.0;
	static constexpr double FRAME_TIME = 1.0 / 60.0;

	NES();

	void LoadROM(const std::filesystem::path& path);

	void Reset();

	void StepFrame();

	const u8* GetFramebuffer() const;

	// Cycles CPU once and PPU 3 times
	void Update();

	void SetButtonsState(u8 state);

private:
	CPU m_Cpu{};
	Cartridge m_Cartridge{};
	PPU m_Ppu{};
	CPUBus m_CpuBus{};
	WRAM m_Wram{};
	HardwareController m_Controller{};

	std::unique_ptr<Mapper> m_Mapper{};
};