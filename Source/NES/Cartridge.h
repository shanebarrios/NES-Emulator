#pragma once

#include "Common.h"
#include <filesystem>

enum class MirrorMode : u8
{
	Horizontal,
	Vertical,
	FourScreen
};

inline constexpr usize PRG_ROM_BANK_SIZE = 0x4000;
inline constexpr usize CHR_ROM_BANK_SIZE = 0x2000;
inline constexpr usize RAM_BANK_SIZE = 0x2000;
inline constexpr usize TRAINER_SIZE = 0x200;

using PrgRomBank = Memory<PRG_ROM_BANK_SIZE>;
using ChrRomBank = Memory<CHR_ROM_BANK_SIZE>;
using RamBank = Memory<RAM_BANK_SIZE>;

class Cartridge
{
public:
	Cartridge() = default;
	~Cartridge();

	void LoadFromFile(const std::filesystem::path& path);

	u8 ReadPrgRom(u8 bank, u16 offset) const;

	u8 ReadChrRom(u8 bank, u16 offset) const;

	u8 ReadRam(u8 bank, u16 offset) const;
	
	void WriteRam(u8 bank, u16 offset, u8 data);

	MirrorMode GetMirrorMode() const { return m_MirrorMode; }

	u8 GetNumPrgRomBanks() const { return m_NumPrgRomBanks; }

	u8 GetNumChrRomBanks() const { return m_NumChrRomBanks; }

	u8 GetNumRamBanks() const { return m_NumRamBanks; }

	u8 GetMapperNumber() const { return m_MapperNumber; }

private:
	PrgRomBank* m_PrgRomBanks = nullptr;
	ChrRomBank* m_ChrRomBanks = nullptr;
	RamBank* m_RamBanks = nullptr;
	u8* m_Trainer = nullptr;

	u8 m_NumPrgRomBanks = 0;
	u8 m_NumChrRomBanks = 0;
	u8 m_NumRamBanks = 0;

	MirrorMode m_MirrorMode = MirrorMode::Horizontal;
	u8 m_MapperNumber = 0;

	bool m_HasRam = false;
	bool m_HasTrainer = false;
};