#pragma once

#include "Common.h"
#include <filesystem>

enum class MirrorMode
{
	Horizontal,
	Vertical,
	FourScreen
};

class Cartridge
{
public:
	Cartridge() = default;

	void LoadROM(const std::filesystem::path& path);

private:
	Memory<0x4000>* m_PrgRomBanks;
	Memory<0x2000>* m_ChrRomBanks;
	usize m_NumPrgRomBanks;

	MirrorMode m_MirrorMode;
};