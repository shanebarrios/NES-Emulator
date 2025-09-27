#pragma once

#include "CPU.h"
#include "Bus.h"
#include "CPU.h"
#include "Cartridge.h"

#include <array>
#include <filesystem>

using RAM = std::array<uint8_t, 0x800>;

class System
{
public:
	System() = default;

	void Init();

	void LoadROM(const std::filesystem::path& path);

	void PerformCycle();

private:
	CPU m_CPU{};
	RAM m_RAM{};
	Cartridge m_Cartridge{};
	Bus m_Bus{};
};