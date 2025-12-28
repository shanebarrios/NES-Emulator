#include "CPUBus.h"

#include "Mapper.h"
#include "PPU.h"
#include "HardwareController.h"
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

CPUBus::CPUBus(Mapper* mapper, PPU* ppu, u8* ram, HardwareController* controller) : 
	m_Mapper{ mapper }, 
	m_Ppu{ ppu }, 
	m_Ram{ram },
	m_Controller{controller}
{
}

void CPUBus::Attach(Mapper* mapper, PPU* ppu, u8* ram, HardwareController* controller)
{
	m_Mapper = mapper;
	m_Ppu = ppu;
	m_Ram = ram;
	m_Controller = controller;
}

u8 CPUBus::Read(u16 addr)
{
	u8 read = m_LastRead;

	if (addr < 0x2000)
	{
		read = m_Ram[addr & 0x7FF];
	}
	else if (addr < 0x4000)
	{
		const u16 mirrored = 0x2000 | (addr & 0x7);
		switch (mirrored)
		{
		case PPUSTATUS:
			read = m_Ppu->GetStatus();
			break;
		case OAMDATA:
			read = m_Ppu->GetOAMData();
			break;
		case PPUDATA:
			read = m_Ppu->GetData();
			break;
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
			// upper bits are open bus
			read = (m_LastRead & 0b11100000) | m_Controller->ReadBit();
			break;
		default:
			break;
			// open bus
		}
	}
	else
	{
		read = m_Mapper->CpuRead(addr);
	}
	m_LastRead = read;
	return read;
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
			//Memory<256> buf;
			//const u16 base = val * 0x100;
			//for (usize offset = 0; offset < 256; offset++)
			//{
			//	buf[offset] = Read(base + offset);
			//}
			//m_Ppu->DMA(buf);
			
			// handled by CPU now
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

void CPUBus::PPUDirectWrite(u8 val)
{
	m_Ppu->DirectOAMWrite(val);
}
