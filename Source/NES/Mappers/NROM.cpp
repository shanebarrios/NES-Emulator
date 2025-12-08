#include "../Mapper.h"

u8 NROM::CpuRead(u16 addr)
{
	if (addr < 0x6000)
	{
		// do nothing
	}
	// Unbanked PRG-RAM
	else if (addr < 0x8000)
	{
		return m_Cartridge->ReadRam(0, addr & 0x1FFF);
	}
	// First 16 KiB of PRG-ROM
	else if (addr < 0xC000)
	{
		return m_Cartridge->ReadPrgRom(0, addr & 0x3FFF);
	}
	// Last 16 KiB of PRG-ROM
	else if (addr <= 0xFFFF)
	{
		const u8 bank = (m_Cartridge->GetNumPrgRomBanks() == 1) ? 0 : 1;

		return m_Cartridge->ReadPrgRom(bank, addr & 0x3FFF);
	}
	return 0;
}

void NROM::CpuWrite(u16 addr, u8 data)
{
	// Only writes to RAM are allowed
	if (addr >= 0x6000 && addr < 0x8000)
	{
		m_Cartridge->WriteRam(0, addr & 0x1FFF, data);
	}
}

u8 NROM::PpuRead(u16 addr)
{
	if (addr < 0x2000)
	{
		return m_Cartridge->ReadChrRom(0, addr);
	}
	return 0;
}
