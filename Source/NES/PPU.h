#pragma once

#include <array>
#include "../Core/Common.h"
#include "Mapper.h"

class Cartridge;
class CPUBus;

class PPU
{
public:
	PPU();

	PPU(Mapper* mapper);

	void Attach(Mapper* mapper);

	void SetCtrl(u8 data);

	void SetMask(u8 data);

	u8 GetStatus() const;

	void SetOAMAddr(u8 data);

	u8 GetOAMData() const;

	void SetOAMData(u8 data);

	void SetScroll(u8 data);

	void SetAddr(u8 data);

	u8 GetData() const;

	void SetData(u8 data);

	void WriteToOAM(u8 offset, u8 data);

private:
	u8 Read(u16 addr) const;

	Memory<0x800> m_Vram;
	Memory<0x100> m_Oam;
	Memory<0x20> m_Palette;
	
	Mapper* m_Mapper = nullptr;

	u8 m_CtrlReg1 = 0;
	u8 m_CtrlReg2 = 0;
	u8 m_StatusReg = 0;
	u8 m_SprRamAddrReg = 0;
};