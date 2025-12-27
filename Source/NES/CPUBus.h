#pragma once

#include "../Core/Common.h"

class Mapper;
class PPU;
class VirtualController;

class CPUBus
{
public:
	CPUBus() = default;

	CPUBus(Mapper* mapper, PPU* ppu, u8* ram, VirtualController* controller);

	void Attach(Mapper* mapper, PPU* ppu, u8* ram, VirtualController* controller);

	u8 Read(u16 addr);

	void Write(u16 addr, u8 val);
	
	void PPUDirectWrite(u8 val);

private:
	Mapper* m_Mapper = nullptr;
	PPU* m_Ppu = nullptr;
	u8* m_Ram = nullptr;
	VirtualController* m_Controller = nullptr;

	u8 m_LastRead = 0;
};