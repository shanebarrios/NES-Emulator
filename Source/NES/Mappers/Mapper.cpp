#include "../Mapper.h"

#include "../NES.h"
#include "../Cartridge.h"

std::optional<u16> Mapper::NametableMirror(u16 offset)
{
	const u16 tableOffset = offset & 0x03FF;
	u16 table = (offset >> 10) & 0x3;

	switch (GetMirrorMode())
	{
	case MirrorMode::Horizontal:
		table >>= 1;
		break;
	case MirrorMode::Vertical:
		table &= 1;
		break;
	case MirrorMode::SingleScreenLower:
		table = 0;
		break;
	case MirrorMode::SingleScreenUpper:
		table = 1;
		break;
	case MirrorMode::FourScreen:
		return std::nullopt;
	}
	return (table << 10) | tableOffset;
}