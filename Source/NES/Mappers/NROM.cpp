#include "../Mapper.h"
#include "../../Core/Logger.h"

u8 NROM::CpuRead(u16 addr)
{
	if (addr < 0x6000)
	{
		// do nothing
	}
	// Unbanked PRG-RAM
	else if (addr < 0x8000)
	{
		return m_Cartridge->ReadPrgRam(addr & 0x1FFF);
	}
	// PRG_ROM
	else 
	{
		return m_Cartridge->ReadPrgRom(addr & 0x7FFF);
	}
	LOG_VERBOSE("Invalid CPU read from %hx", addr);
	return 0;
}

void NROM::CpuWrite(u16 addr, u8 data)
{
	// Only writes to RAM are allowed
	if (addr >= 0x6000 && addr < 0x8000)
	{
		m_Cartridge->WritePrgRam(addr & 0x1FFF, data);
		return;
	}
	LOG_VERBOSE("Invalid CPU write to %hx", addr);
}

u8 NROM::PpuRead(u16 addr)
{
	if (addr < 0x2000)
	{
		if (m_Cartridge->GetChrRomSize() == 0)
		{
			return m_Cartridge->ReadChrRam(addr);
		}
		else
		{
			return m_Cartridge->ReadChrRom(addr);
		}
	}
	LOG_VERBOSE("Invalid PPU read from %hx", addr);
	return 0;
}


void NROM::PpuWrite(u16 addr, u8 data)
{
	if (m_Cartridge->GetChrRomSize() == 0)
	{
		m_Cartridge->WriteChrRam(addr, data);
		return;
	}
	LOG_VERBOSE("Invalid PPU write to %hx", addr);
}