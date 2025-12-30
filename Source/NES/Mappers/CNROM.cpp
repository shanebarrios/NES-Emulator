#include "../Mapper.h"
#include "../../Core/Logger.h"
#include "../Cartridge.h"

void CNROM::Init(Cartridge* cartridge, CPU* cpu) 
{
	m_Cartridge = cartridge;
	m_Cpu = cpu;
	m_MirrorMode = m_Cartridge->GetMirrorMode();
}

std::optional<u8> CNROM::CpuRead(u16 addr)
{
	if (addr < 0x6000)
	{
		// do nothing
	}
	// Unbanked PRG-RAM
	else if (addr < 0x8000)
	{
		return m_Cartridge->ReadPrgRam(addr & 0x7FF);
	}
	// 32 KiB unbanked PRG-ROM
	else
	{
		return m_Cartridge->ReadPrgRom(addr & 0x7FFF);
	}
	LOG_VERBOSE("Invalid CPU read from %hx", addr);
	return std::nullopt;
}

void CNROM::CpuWrite(u16 addr, u8 data)
{
	if (addr >= 0x6000 && addr < 0x8000)
	{
		m_Cartridge->WritePrgRam(addr & 0x7FF, data);
		return;
	}
	else if (addr >= 0x8000)
	{
		m_ChrRomBank = data & 0b11;
	}
	LOG_VERBOSE("Invalid CPU write to %hx", addr);
}

std::optional<u8> CNROM::PpuRead(u16 addr)
{
	if (addr < 0x2000)
	{
		return m_Cartridge->ReadChr(m_ChrRomBank * 0x2000 + addr);
	}
	LOG_VERBOSE("Invalid PPU read from %hx", addr);
	return std::nullopt;
}


void CNROM::PpuWrite(u16 addr, u8 data)
{
	// no writes allowed, do nothing
	LOG_VERBOSE("Invalid PPU write to %hx", addr);
}