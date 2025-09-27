#include "Bus.h"

#include <fstream>
#include <cstring>
#include <stdexcept>
#include "Common.h"
#include "CPU.h"
#include "Cartridge.h"

void Bus::Attach(CPU* _cpu, Cartridge* cartridge, uint8_t* ram)
{
	m_CPU = _cpu;
	m_Cartridge = cartridge;
	m_RAM = ram;
}

uint8_t Bus::Read(uint16_t addr) const
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
		return m_Cartridge->ReadRAM(addr % 0x2000);
	}
	else
	{
		return m_Cartridge->ReadROM(addr % 0x8000);
	}
	ASSERT_MSG(false, "Unsupported memory read!");
	return 0;
}

void Bus::Write(uint16_t addr, uint8_t val)
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
