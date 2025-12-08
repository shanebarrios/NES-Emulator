#include "CPUBus.h"

#include "Common.h"
#include "MMC.h"

static constexpr u16 PPUCTRL = 0x2000;
static constexpr u16 PPUMASK = 0x2001;
static constexpr u16 PPUSTATUS = 0x2002;
static constexpr u16 OAMADDR = 0x2003;
static constexpr u16 OAMDATA = 0x2004;
static constexpr u16 PPUSCROLL = 0x2005;
static constexpr u16 PPUADDR = 0x2006;
static constexpr u16 PPUDATA = 0x2007;
static constexpr u16 OAMDMA = 0x4014;

#define PPU_IO_MIRROR(addr) (0x2000 | (addr & 0xF))

u8 CPUBus::Read(u16 addr)
{
	if (addr < 0x2000)
	{
		return m_Ram[addr & 0x7FF];
	}
	else if (addr < 0x4000)
	{
		switch (PPU_IO_MIRROR(addr))
		{
		case PPUCTRL:

		}
	}
	else if (addr < 0x4020)
	{
		// APU and I/O functionality, normally disabled
	}
	else
	{
		return m_Mmc->CpuRead(addr);
	}
	return 0;

}

void CPUBus::Write(u16 addr, u8 val)
{
	if (addr < 0x2000)
	{
		m_Ram[addr % 0x800] = val;
	}
	else if (addr < 0x4000)
	{
		// PPU, do nothing for now
	}
	else if (addr < 0x4020)
	{
		// APU and I/O functionality, normally disabled
	}
	else
	{
		m_Mmc->CpuWrite(addr, val);
	}
}
