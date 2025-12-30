#include "../Mapper.h"
#include "../Cartridge.h"
#include "../../Core/Logger.h"
#include "../CPU.h"

void MMC1::Init(Cartridge* cartridge, CPU* cpu) 
{
	m_Cartridge = cartridge;
	m_Cpu = cpu;
	m_MirrorMode = MirrorMode::SingleScreenLower;
}

u8 MMC1::CpuRead(u16 addr)
{
	if (addr < 0x6000)
	{
		// do nothing
	}
	// Unbanked PRG-RAM
	else if (addr < 0x8000)
	{
		// TODO: open bus
		if (m_PrgRamDisabled)
		{
			return 0;
		}
		else
		{
			return m_Cartridge->ReadPrgRam(addr & 0x1FFF);
		}
	}
	// PRG_ROM bank 0
	else if (addr < 0xC000)
	{
		usize offset = addr & 0x3FFF;
		if (m_PrgRomBankMode != PrgRomBankMode::Fix0)
		{
			offset |= m_PrgRomBank << 14u;
		}
		return m_Cartridge->ReadPrgRom(offset);
	}
	// PRG_ROM bank 1
	else
	{
		usize offset = addr & 0x3FFF;

		const u8 lastBank = (m_Cartridge->GetPrgRomSize() >> 14) - 1;

		switch (m_PrgRomBankMode)
		{
		case PrgRomBankMode::SwitchBoth0:
		case PrgRomBankMode::SwitchBoth1:
			offset |= ((m_PrgRomBank | 1u) << 14u);
			break;
		case PrgRomBankMode::Fix0:
			offset |= (m_PrgRomBank << 14u);
			break;
		// last bank
		case PrgRomBankMode::Fix1:
			offset |= lastBank * 0x4000;
			break;
		}
		return m_Cartridge->ReadPrgRom(offset);
	}
	LOG_VERBOSE("Invalid CPU read from %hx", addr);
	return 0;
}

void MMC1::CpuWrite(u16 addr, u8 data)
{
	// Only writes to RAM are allowed
	if (addr >= 0x6000 && addr < 0x8000)
	{
		m_Cartridge->WritePrgRam(addr & 0x1FFF, data);
		return;
	}
	else if (addr >= 0x8000)
	{
		// consecutive cycle writes ignored
		if (m_Cpu->GetCycle() == m_LastCpuWrite + 1)
		{
			return;
		}
		if (data & 0x80)
		{
			m_ShiftReg = 0x10;
		}
		else
		{
			const bool complete = m_ShiftReg & 1;
			m_ShiftReg = ((data & 1) << 4) | (m_ShiftReg >> 1);
			if (complete)
			{
				UpdateBank(addr);
				m_ShiftReg = 0x10;
			}
		}
		m_LastCpuWrite = m_Cpu->GetCycle();
		return;
	}
	LOG_VERBOSE("Invalid CPU write to %hx", addr);
}

u8 MMC1::PpuRead(u16 addr)
{
	if (addr < 0x1000)
	{
		const usize offset = (addr & 0xFFF) | (m_ChrBank0 << 12);
		return m_Cartridge->ReadChr(offset);
	}
	else if (addr < 0x2000)
	{
		const usize offset = (addr & 0xFFF) | (m_ChrBank1 << 12);
		return m_Cartridge->ReadChr(offset);
	}
	LOG_VERBOSE("Invalid PPU read from %hx", addr);
	return 0;
}


void MMC1::PpuWrite(u16 addr, u8 data)
{
	if (addr < 0x1000)
	{
		const usize offset = (addr & 0xFFF) | (m_ChrBank0 << 12);
		m_Cartridge->WriteChr(offset, data);
		return;
	}
	else if (addr < 0x2000)
	{
		const usize offset = (addr & 0xFFF) | (m_ChrBank1 << 12);
		m_Cartridge->WriteChr(offset, data);
		return;
	}
	LOG_VERBOSE("Invalid PPU write to %hx", addr);
}

void MMC1::UpdateBank(u16 addr)
{
	const u8 select = (addr >> 13) & 0b11;
	
	switch (select)
	{
	case 0: UpdateControl(); break;
	case 1: UpdateChrBank0(); break;
	case 2: UpdateChrBank1(); break;
	case 3: UpdatePrgBank(); break;
	}
}

void MMC1::UpdateControl()
{
	const u8 val = m_ShiftReg;
	const u8 arrangment = val & 0b11;
	MirrorMode mirrorMode;
	switch (arrangment)
	{
	case 0: mirrorMode = MirrorMode::SingleScreenLower; break;
	case 1: mirrorMode = MirrorMode::SingleScreenUpper; break;
	case 2: mirrorMode = MirrorMode::Vertical; break;
	case 3: mirrorMode = MirrorMode::Horizontal; break;
	}
	m_MirrorMode = mirrorMode;
	m_PrgRomBankMode = static_cast<PrgRomBankMode>((val >> 2) & 0b11);
	m_ChrRomBankMode = static_cast<ChrRomBankMode>(val >> 4);
}

void MMC1::UpdateChrBank0()
{
	const u8 val = m_ShiftReg;
	switch (m_ChrRomBankMode)
	{
	case ChrRomBankMode::SwitchBoth:
		m_ChrBank0 = val & ~1u;
		m_ChrBank1 = val | 1u;
		break;
	case ChrRomBankMode::SwitchSeparate:
		m_ChrBank0 = val;
		break;
	}
}

void MMC1::UpdateChrBank1()
{
	const u8 val = m_ShiftReg;
	switch (m_ChrRomBankMode)
	{
	case ChrRomBankMode::SwitchBoth:
		// do nothing
		break;
	case ChrRomBankMode::SwitchSeparate:
		m_ChrBank1 = val;
		break;
	}
}

void MMC1::UpdatePrgBank()
{
	const u8 num = m_ShiftReg & 0b1111;
	switch (m_PrgRomBankMode)
	{
	case PrgRomBankMode::SwitchBoth0:
	case PrgRomBankMode::SwitchBoth1:
		m_PrgRomBank = num & ~1u;
		break;
	case PrgRomBankMode::Fix0:
		m_PrgRomBank = num;
		break;
	case PrgRomBankMode::Fix1:
		m_PrgRomBank = num;
		break;
	}
	m_PrgRamDisabled = m_ShiftReg & (1 << 4);
}