#include "PPU.h"
#include "MMC.h"

PPU::PPU(MMC* mmc) : m_Mmc{mmc}
{

}

u8 PPU::Read(u16 addr) const
{
	if (addr < 0x2000)
	{
		return m_Mmc->PpuRead(addr);
	}
	else if (addr < 0x3F00)
	{
		return m_Vram[addr & 0x7FF];
	}
	else if (addr < 0x3F20)
	{
		// divisible by 4, mirror to 0x3F00
		if (!(addr & 0x3))
		{
			return m_Palette[0];
		}
		return m_Palette[addr & 0x1F];
	}
}