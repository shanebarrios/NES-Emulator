#pragma once

#include "Common.h"

class Cartridge;
class MMC;

class CPUBus
{
public:
	CPUBus(MMC* mmc, u8* ram) :
		m_Mmc{ mmc }, m_Ram{ ram } {}

	u8 Read(u16 addr);

	void Write(u16 addr, u8 val);

private:
	MMC* m_Mmc;
	u8* m_Ram;
};