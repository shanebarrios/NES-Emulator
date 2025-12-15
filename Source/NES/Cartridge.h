#pragma once

#include "../Core/Common.h"
#include <filesystem>
#include <memory>

enum class MirrorMode : u8
{
	Horizontal,
	Vertical,
	SingleScreen,
	FourScreen
};

class Cartridge
{
public:
	Cartridge() = default;

	void LoadFromFile(const std::filesystem::path& path);

	u8 ReadPrgRom(u16 offset) const;

	u8 ReadChrRom(u16 offset) const;

	u8 ReadPrgRam(u16 offset) const;

	u8 ReadChrRam(u16 offset) const;
	
	void WritePrgRam(u16 offset, u8 data);

	void WriteChrRam(u16 offset, u8 data);

	MirrorMode GetMirrorMode() const { return m_MirrorMode; }

	usize GetPrgRomSize() const { return m_PrgRomSize; }

	usize GetChrRomSize() const { return m_ChrRomSize; }

	usize GetPrgRamSize() const { return m_PrgRamSize; }

	u8 GetMapperNumber() const { return m_MapperNumber; }

private:
	std::unique_ptr<u8[]> m_PrgRom = nullptr;
	std::unique_ptr<u8[]> m_ChrRom = nullptr;
	std::unique_ptr<u8[]> m_PrgRam = nullptr;
	std::unique_ptr<u8[]> m_ChrRam = nullptr;

	std::unique_ptr<u8[]> m_Trainer = nullptr;

	usize m_PrgRomSize = 0;
	usize m_ChrRomSize = 0;
	usize m_PrgRamSize = 0;

	MirrorMode m_MirrorMode = MirrorMode::Horizontal;
	u8 m_MapperNumber = 0;

	bool m_HasPrgRam = false;
	bool m_HasTrainer = false;
};