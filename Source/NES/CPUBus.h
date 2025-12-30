#pragma once

#include "../Core/Common.h"

class Mapper;
class PPU;
class HardwareController;

class CPUBus
{
public:
	CPUBus() = default;

	CPUBus(Mapper* mapper, PPU* ppu, u8* ram, HardwareController* controller);

	void Attach(Mapper* mapper, PPU* ppu, u8* ram, HardwareController* controller);

	u8 Read(u16 addr);

	void Write(u16 addr, u8 val);
	
	void PPUDirectWrite(u8 val);

private:
	Mapper* m_Mapper = nullptr;
	PPU* m_Ppu = nullptr;
	u8* m_Ram = nullptr;
	HardwareController* m_Controller = nullptr;

	u8 m_OpenBus = 0;
};