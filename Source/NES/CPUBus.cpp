#include "CPUBus.h"

#include "Mapper.h"
#include "PPU.h"
#include "HardwareController.h"
#include "../Core/Logger.h"

enum IORegisters : u16
{
	PPUCTRL = 0x2000,
	PPUMASK,
	PPUSTATUS,
	OAMADDR,
	OAMDATA,
	PPUSCROLL,
	PPUADDR,
	PPUDATA,
	OAMDMA = 0x4014,
	JOY1 = 0x4016,
	JOY2
};


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

// TODO: check if switching on bits is faster than conditionals
u8 CPUBus::Read(u16 addr)
{
	u8 read = m_OpenBus;

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
			// open bus
			read = m_Ppu->GetIOBus();
			break;
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
			read = (m_OpenBus & 0b11100000) | m_Controller->ReadBit();
			break;
		default:
			break;
			// open bus
		}
	}
	else
	{
		auto readOptional = m_Mapper->CpuRead(addr);
		if (readOptional)
		{
			read = *readOptional;
		}
	}
	m_OpenBus = read;
	return read;
}

void CPUBus::Write(u16 addr, u8 val)
{
	m_OpenBus = val;

	if (addr < 0x2000)
	{
		m_Ram[addr & 0x7FF] = val;
		return;
	}
	else if (addr < 0x4000)
	{
		const u16 mirrored = 0x2000 | (addr & 0x7);
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
			// invalid write
			m_Ppu->SetIOBus(val);
			break;
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
