#pragma once

#include <array>
#include "Common.h"
#include "MMC.h"

class Cartridge;

class PPU
{
public:
	PPU(MMC* mmc);

	void SetControl1(u8 data);

	void SetControl2(u8 data);

	u8 GetStatus() const;

	void SetOAMAddr(u8 data);

	void SetOAMData(u8 data);

	void SetScroll(u8 data);

	void SetAddr(u8 data);

	void SetData(u8 data);

	void DMA(u8* mem);

private:
	u8 Read(u16 addr) const;

	Memory<0x800> m_Vram;
	Memory<0x100> m_Spram;
	Memory<0x20> m_Palette;
	
	MMC* m_Mmc = nullptr;

	u8 m_CtrlReg1 = 0;
	u8 m_CtrlReg2 = 0;
	u8 m_StatusReg = 0;
	u8 m_SprRamAddrReg = 0;

	
};