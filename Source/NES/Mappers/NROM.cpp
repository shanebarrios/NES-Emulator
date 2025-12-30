#include "../Mapper.h"
#include "../../Core/Logger.h"
#include "../Cartridge.h"

void NROM::Init(Cartridge* cartridge, CPU* cpu)
{
	m_Cartridge = cartridge;
	m_Cpu = cpu;
	m_MirrorMode = m_Cartridge->GetMirrorMode();
}


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
		return m_Cartridge->ReadChr(addr);
	}
	LOG_VERBOSE("Invalid PPU read from %hx", addr);
	return 0;
}


void NROM::PpuWrite(u16 addr, u8 data)
{
	m_Cartridge->WriteChr(addr, data);
	LOG_VERBOSE("Invalid PPU write to %hx", addr);
}