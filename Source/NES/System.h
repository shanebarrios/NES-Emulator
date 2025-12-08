#pragma once

#include "Common.h"
#include "CPU.h"
#include "CPUBus.h"
#include "Cartridge.h"
#include "MMC.h"

#include <filesystem>
#include <memory>

inline constexpr usize SYSTEM_RAM_SIZE = 0x800;

class System
{
public:
	System();
	~System();

	void LoadROM(const std::filesystem::path& path);

	void PerformCycle();

private:
	std::unique_ptr<CPU> m_Cpu = nullptr;
	std::unique_ptr<Cartridge> m_Cartridge = nullptr;
	std::unique_ptr<MMC> m_Mmc = nullptr;
	std::unique_ptr<CPUBus> m_CpuBus = nullptr;
	std::unique_ptr<u8[]> m_SystemRam = nullptr;
};