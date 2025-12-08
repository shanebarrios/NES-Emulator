#include "PPU.h"
#include "Mapper.h"

PPU::PPU()
{

}

PPU::PPU(Mapper* mapper) : m_Mapper{mapper}
{

}

void PPU::Attach(Mapper* mapper)
{
	m_Mapper = mapper;
}

void PPU::SetCtrl(u8 data)
{

}

void PPU::SetMask(u8 data)
{

}

u8 PPU::GetStatus() const
{
	return 0;
}

void PPU::SetOAMAddr(u8 data)
{

}

u8 PPU::GetOAMData() const
{
	return 0;
}

void PPU::SetOAMData(u8 data)
{

}

void PPU::SetScroll(u8 data)
{

}

void PPU::SetAddr(u8 data)
{

}

u8 PPU::GetData() const
{
	return 0;
}

void PPU::SetData(u8 data)
{

}

void PPU::WriteToOAM(u8 offset, u8 data)
{
	m_Oam[offset] = data;
}

u8 PPU::Read(u16 addr) const
{
	if (addr < 0x2000)
	{
		return m_Mapper->PpuRead(addr);
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