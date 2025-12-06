#include "CPUBus.h"

#include <fstream>
#include <cstring>
#include <stdexcept>
#include "Common.h"
#include "CPU.h"
#include "Cartridge.h"

void CPUBus::Attach(CPU* _cpu, Cartridge* cartridge, u8* ram)
{
	m_CPU = _cpu;
	m_Cartridge = cartridge;
	m_RAM = ram;
}

u8 CPUBus::Read(u16 addr) const
{
	if (addr < 0x2000)
	{
		return m_RAM[addr % 0x800];
	}
	else if (addr < 0x4000)
	{
		// PPU, do nothing for now
	}
	else if (addr < 0x4020)
	{
		// APU and I/O functionality, normally disabled
	}
	else if (addr < 0x6000)
	{
		
	}
	else if (addr < 0x8000)
	{
		return m_Cartridge->ReadSRAM(addr % 0x2000);
	}
	else
	{
		return m_Cartridge->ReadROM(addr % 0x8000);
	}
	ASSERT_MSG(false, "Unsupported memory read!");
	return 0;
}

void CPUBus::Write(u16 addr, u8 val)
{
	if (addr < 0x2000)
	{
		m_RAM[addr % 0x800] = val;
	}
	else if (addr < 0x4000)
	{
		// PPU, do nothing for now
	}
	else if (addr < 0x4020)
	{
		// APU and I/O functionality, normally disabled
	}
	else if (addr < 0x8000)
	{
		// Unmapped, do nothing for now
	}
	else
	{
		ASSERT_MSG(false, "Invalid memory write to cartridge ROM!");
	}
}
