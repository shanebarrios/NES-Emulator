#pragma once

#include "Common.h"
#include "Cartridge.h"

class MMC
{
public:
	MMC(Cartridge* cartridge) { m_Cartridge = cartridge; }

	void AttachCartridge(Cartridge* cartridge) { m_Cartridge = cartridge; }

	virtual ~MMC() = default;

	virtual u8 CpuRead(u16 addr) = 0;

	virtual void CpuWrite(u16 addr, u8 data) = 0;

	virtual u8 PpuRead(u16 addr) = 0;

	virtual MirrorMode GetMirrorMode() const { return m_Cartridge->GetMirrorMode(); }

protected:
	Cartridge* m_Cartridge;
};

class Mapper0 : public MMC
{
public:
	Mapper0(Cartridge* cartridge) : MMC{ cartridge } {}

	u8 CpuRead(u16 addr) override;

	void CpuWrite(u16 addr, u8 data) override;

	u8 PpuRead(u16 addr) override;
};