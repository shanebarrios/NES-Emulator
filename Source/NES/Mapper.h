#pragma once

#include "../Core/Common.h"
#include "Cartridge.h"
#include <optional>

enum class MirrorMode : u8;
class Cartridge;
class CPU;

#define MAPPER_BASE_PUBLIC_INTERFACE(name) \
	name() = default; \
	void Init(Cartridge* cartridge, CPU* cpu) override; \
	std::optional<u8> CpuRead(u16 addr) override; \
	void CpuWrite(u16 addr, u8 data) override; \
	std::optional<u8> PpuRead(u16 addr) override; \
	void PpuWrite(u16 addr, u8 data) override; \

class Mapper
{
public:
	Mapper() = default;

	virtual ~Mapper() = default;

	virtual void Init(Cartridge* cartridge, CPU* cpu) = 0;

	// Returns std::nullopt on open bus
	virtual std::optional<u8> CpuRead(u16 addr) = 0;

	virtual void CpuWrite(u16 addr, u8 data) = 0;

	// Returns std::nullopt on open bus
	virtual std::optional<u8> PpuRead(u16 addr) = 0;

	virtual void PpuWrite(u16 addr, u8 data) = 0;

	// Takes in nametable offset, returns mirrored VRAM addr 
	// or std::nullopt if mapper redirects to cartridge
	virtual std::optional<u16> NametableMirror(u16 offset);

	MirrorMode GetMirrorMode() const { return m_MirrorMode; }

protected:
	Cartridge* m_Cartridge = nullptr;
	CPU* m_Cpu = nullptr;
	MirrorMode m_MirrorMode = MirrorMode::SingleScreenLower;
};

class NROM : public Mapper
{
public:
	MAPPER_BASE_PUBLIC_INTERFACE(NROM)
};

class CNROM : public Mapper
{
public:
	MAPPER_BASE_PUBLIC_INTERFACE(CNROM)

private:
	u8 m_ChrRomBank = 0;
};

class MMC1 : public Mapper
{
public:
	MAPPER_BASE_PUBLIC_INTERFACE(MMC1)

private:
	void UpdateBank(u16 addr);
	void UpdateControl();
	void UpdateChrBank0();
	void UpdateChrBank1();
	void UpdatePrgBank();

private:
	enum class PrgRomBankMode
	{
		SwitchBoth0 = 0,
		SwitchBoth1,
		Fix0,
		Fix1
	};

	enum class ChrRomBankMode
	{
		SwitchBoth = 0,
		SwitchSeparate
	};

	u8 m_ShiftReg = 0x10;

	PrgRomBankMode m_PrgRomBankMode = PrgRomBankMode::Fix1;
	ChrRomBankMode m_ChrRomBankMode = ChrRomBankMode::SwitchBoth;

	u8 m_ChrBank0 = 0;
	u8 m_ChrBank1 = 0;
	u8 m_PrgRomBank = 0;

	bool m_PrgRamDisabled = false;

	u64 m_LastCpuWrite = 0;
};