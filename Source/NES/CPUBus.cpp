#include "CPUBus.h"

#include "Mapper.h"
#include "PPU.h"
#include "VirtualController.h"
#include "../Core/Logger.h"

static constexpr u16 PPUCTRL = 0x2000;
static constexpr u16 PPUMASK = 0x2001;
static constexpr u16 PPUSTATUS = 0x2002;
static constexpr u16 OAMADDR = 0x2003;
static constexpr u16 OAMDATA = 0x2004;
static constexpr u16 PPUSCROLL = 0x2005;
static constexpr u16 PPUADDR = 0x2006;
static constexpr u16 PPUDATA = 0x2007;
static constexpr u16 OAMDMA = 0x4014;
static constexpr u16 JOY1 = 0x4016;
static constexpr u16 JOY2 = 0x4017;

CPUBus::CPUBus(Mapper* mapper, PPU* ppu, u8* ram, VirtualController* controller) : 
	m_Mapper{ mapper }, 
	m_Ppu{ ppu }, 
	m_Ram{ram },
	m_Controller{controller}
{
}

void CPUBus::Attach(Mapper* mapper, PPU* ppu, u8* ram, VirtualController* controller)
{
	m_Mapper = mapper;
	m_Ppu = ppu;
	m_Ram = ram;
	m_Controller = controller;
}

u8 CPUBus::Read(u16 addr)
{
	if (addr < 0x2000)
	{
		return m_Ram[addr & 0x7FF];
	}
	else if (addr < 0x4000)
	{
		const u16 mirrored = 0x2000 | (addr & 0xF);
		switch (mirrored)
		{
		case PPUSTATUS:
			return m_Ppu->GetStatus();
		case OAMDATA:
			return m_Ppu->GetOAMData();
		case PPUDATA:
			return m_Ppu->GetData();
		default:
			break;
			// open bus
		}
	}
	else if (addr < 0x4020)
	{
		// TODO: handle open bus for upper bits of controller
		// Also add APU here
		switch (addr)
		{
		case JOY1:
			return m_Controller->ReadBit();
		default:
			break;
			// open bus
		}
	}
	else
	{
		return m_Mapper->CpuRead(addr);
	}

	LOG_VERBOSE("Invalid CPU read from %hx", addr);
	return 0;

}

void CPUBus::Write(u16 addr, u8 val)
{
	if (addr < 0x2000)
	{
		m_Ram[addr & 0x7FF] = val;
		return;
	}
	else if (addr < 0x4000)
	{
		const u16 mirrored = 0x2000 | (addr & 0xF);
		switch (mirrored)
		{
		case PPUCTRL:
			m_Ppu->SetCtrl(val);
			return;
		case PPUMASK:
			m_Ppu->SetMask(val);
			return;
		case OAMADDR:
			m_Ppu->SetOAMAddr(val);
			return;
		case OAMDATA:
			m_Ppu->SetOAMData(val);
			return;
		case PPUSCROLL:
			m_Ppu->SetScroll(val);
			return;
		case PPUADDR:
			m_Ppu->SetAddr(val);
			return;
		case PPUDATA:
			m_Ppu->SetData(val);
			return;
		default:
			break;
			// invalid write
		}
	}
	else if (addr < 0x4020)
	{
		// APU and I/O functionality, most are normally disabled
		switch (addr)
		{
		case OAMDMA: 
		{
			Memory<256> buf;
			const u16 base = val * 0x100;
			for (usize offset = 0; offset < 256; offset++)
			{
				buf[offset] = Read(base + offset);
			}
			m_Ppu->DMA(buf);
			return;
		}
		case JOY1:
			m_Controller->Write(val & 1);
			break;
		default:
			break;
			// invalid write
		}
	}
	else
	{
		m_Mapper->CpuWrite(addr, val);
		return;
	}

	LOG_VERBOSE("Invalid CPU write to %hx", addr);
}
