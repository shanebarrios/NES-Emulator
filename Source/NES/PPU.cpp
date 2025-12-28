#include "PPU.h"
#include "Mapper.h"
#include "CPU.h"
#include "../Core/Logger.h"

static constexpr u8 CTRL_SPRITE_TILE_SELECT_SHIFT = 3;
static constexpr u8 CTRL_BACKGROUND_TILE_SELECT_SHIFT = 4;

static constexpr u8 CTRL_NAMETABLE_MASK = 0b11;
static constexpr u8 CTRL_INCREMENT_MODE_BIT = 1 << 2;
static constexpr u8 CTRL_SPRITE_TILE_SELECT_BIT = 1 << 3;
static constexpr u8 CTRL_BACKGROUND_TILE_SELECT_BIT = 1 << 4;
static constexpr u8 CTRL_SPRITE_SIZE_BIT = 1 << 5;
static constexpr u8 CTRL_MASTER_SLAVE_BIT = 1 << 6;
static constexpr u8 CTRL_NMI_ENABLE_BIT = 1 << 7;

static constexpr u8 MASK_GREYSCALE_BIT = 1;
static constexpr u8 MASK_BACKGROUND_LEFT_COLUMN_ENABLE_BIT = 1 << 1;
static constexpr u8 MASK_SPRITE_LEFT_COLUMN_ENABLE_BIT = 1 << 2;
static constexpr u8 MASK_BACKGROUND_ENABLE_BIT = 1 << 3;
static constexpr u8 MASK_SPRITE_ENABLE_BIT = 1 << 4;
static constexpr u8 MASK_COLOR_EMPHASIS_MASK = 0b111 << 5;

static constexpr u8 STATUS_SPRITE_OVERFLOW_BIT = 1 << 5;
static constexpr u8 STATUS_SPRITE0_HIT_BIT = 1 << 6;
static constexpr u8 STATUS_VBLANK_BIT = 1 << 7;

static constexpr u8 SCROLL_FINE_MASK = 0b111;
static constexpr u8 SCROLL_COARSE_MASK = 0b11111 << 3;

static constexpr u16 INTERNAL_COARSE_X_SCROLL_SHIFT = 0;
static constexpr u16 INTERNAL_COARSE_Y_SCROLL_SHIFT = 5;
static constexpr u16 INTERNAL_NAMETABLE_SELECT_SHIFT = 10;
static constexpr u16 INTERNAL_FINE_Y_SCROLL_SHIFT = 12;

static constexpr u16 INTERNAL_COARSE_X_SCROLL_MASK = 0b11111;
static constexpr u16 INTERNAL_COARSE_Y_SCROLL_MASK = 0b11111 << 5;
static constexpr u16 INTERNAL_NAMETABLE_X_MASK = 1 << 10;
static constexpr u16 INTERNAL_NAMETABLE_Y_MASK = 1 << 11;
static constexpr u16 INTERNAL_NAMETABLE_SELECT_MASK = 0b11 << 10;
static constexpr u16 INTERNAL_FINE_Y_SCROLL_MASK = 0b111 << 12;

static constexpr u8 SPRITE_ATTRIBUTE_PRIORITY_SHIFT = 5;

static constexpr u8 SPRITE_ATTRIBUTE_PALETTE_MASK = 0b11;
static constexpr u8 SPRITE_ATTRIBUTE_UNIMPLEMENTED_MASK = 0b111 << 2;
static constexpr u8 SPRITE_ATTRIBUTE_PRIORITY_BIT = 1 << 5;
static constexpr u8 SPRITE_ATTRIBUTE_FLIP_HORIZONTAL_BIT = 1 << 6;
static constexpr u8 SPRITE_ATTRIBUTE_FLIP_VERTICAL_BIT = 1 << 7;

static constexpr u16 PATTERN_TABLE_BIT_PLANE_SHIFT = 3;
static constexpr u16 PATTERN_TABLE_TILE_SHIFT = 4;
static constexpr u16 PATTERN_TABLE_HALF_SHIFT = 12;

static constexpr u16 SCANLINES_PER_FRAME = 262;
static constexpr u16 POST_RENDER_START = 240;
static constexpr u16 DOTS_PER_SCANLINE = 341;
static constexpr u16 PRERENDER_LINE = 261;

static constexpr u16 SPRITE0_NONE = 0xFFFF;

// note: backdrop color can be overwritten if m_V points to palette RAM (3F00-3FFF)

PPU::PPU() :
	PPU {nullptr, nullptr}
{
	
}

PPU::PPU(CPU* cpu, Mapper* mapper) : 
	m_Cpu {cpu},
	m_Mapper{mapper}
{
	m_Framebuffer = std::make_unique<u8[]>(SCREEN_WIDTH * SCREEN_HEIGHT);
}

void PPU::Attach(CPU* cpu, Mapper* mapper)
{
	m_Cpu = cpu;
	m_Mapper = mapper;
}

void PPU::PerformCycle()
{
	if (m_ScanlineCycle == 0)
	{
		// idle cycle for dot 0, do nothing
	}
	else if (m_Scanline < POST_RENDER_START)
	{
		VisibleCycle();
	}
	else if (m_Scanline == PRERENDER_LINE)
	{
		PrerenderCycle();
	}
	else
	{
		VBlankCycle();
	}

	AdvanceCycle();
}

void PPU::AdvanceCycle()
{
	m_ScanlineCycle++;
	bool oddFrame = m_FrameNumber % 2 == 1;
	// skip a cycle at end of odd frame
	if (m_Scanline == PRERENDER_LINE &&
		oddFrame &&
		m_ScanlineCycle == DOTS_PER_SCANLINE - 1)
	{
		m_ScanlineCycle = 0;
		m_Scanline = 0;
		m_FrameNumber++;
		m_FramebufferReady = false;
	}
	else if (m_ScanlineCycle == DOTS_PER_SCANLINE)
	{
		m_ScanlineCycle = 0;
		m_Scanline++;
		if (m_Scanline == SCANLINES_PER_FRAME)
		{
			m_Scanline = 0;
			m_FrameNumber++;
			m_FramebufferReady = false;
		}
	}
	if (m_Scanline == POST_RENDER_START && m_ScanlineCycle == 0)
	{
		m_FramebufferReady = true;
	}
}

void PPU::Reset()
{
	// Memory
	m_Vram.fill(0);
	m_Palette.fill(0);
	m_Oam.fill(0);
	m_SecondaryOam.fill(0);
	std::memset(m_Framebuffer.get(), 0, SCREEN_WIDTH * SCREEN_HEIGHT);

	m_CtrlReg = m_MaskReg = m_StatusReg = m_OamAddr = m_Scroll = 0;

	// Internal registers
	m_V = m_T = m_X = m_W = 0;

	// Fetch registers
	m_NT = m_AT = m_BGLow = m_BGHigh = 0;

	// Shift registers
	m_BGTileLowShift = m_BGTileHighShift = m_BGPaletteLowShift = m_BGPaletteHighShift = 0;

	m_ReadBuf = m_Scanline = m_ScanlineCycle = m_FrameNumber = 0;

	// Sprite evaluation
	m_SpritePixelBuf.fill({});
	m_Sprite0InRange = false;
}

void PPU::PrerenderCycle()
{
	if (m_ScanlineCycle == 1)
	{
		m_StatusReg &= ~(STATUS_VBLANK_BIT | STATUS_SPRITE0_HIT_BIT | STATUS_SPRITE_OVERFLOW_BIT);
		m_Cpu->SetNMILine(false);
	}

	FetchBackgroundData();

	ShiftRegisters();

	if (m_ScanlineCycle >= 280 && m_ScanlineCycle <= 304 && RenderingEnabled())
	{
		// vert(v) = vert(t)
		const u16 mask =
			INTERNAL_COARSE_Y_SCROLL_MASK |
			INTERNAL_FINE_Y_SCROLL_MASK |
			INTERNAL_NAMETABLE_Y_MASK;
		m_V &= ~mask;
		m_V |= m_T & mask;
	}

	UpdateV();
}

void PPU::VBlankCycle()
{
	// update vblank flag, trigger NMI
	if (m_Scanline == 241 && m_ScanlineCycle == 1)
	{
		m_StatusReg |= STATUS_VBLANK_BIT;
		if (m_CtrlReg & CTRL_NMI_ENABLE_BIT)
		{
			m_Cpu->SetNMILine(true);
		}
	}
}

void PPU::VisibleCycle()
{
	SetPixel();

	FetchBackgroundData();

	EvaluateSprites();

	ShiftRegisters();

	UpdateV();
}

void PPU::SetPixel()
{
	if (m_ScanlineCycle > 256)
	{
		return;
	}

	const usize y = m_Scanline;
	const usize x = m_ScanlineCycle - 1;

	u8 backgroundPixels = 0;

	if ((m_MaskReg & MASK_BACKGROUND_ENABLE_BIT) && 
		(x >= 8 || m_MaskReg & MASK_BACKGROUND_LEFT_COLUMN_ENABLE_BIT))
	{
		const u8 fineX = m_X;
		const u16 bitIndex = 15 - fineX;

		const u8 paletteOffset =
			((m_BGTileLowShift >> bitIndex) & 1) |
			((m_BGTileHighShift >> (bitIndex - 1)) & 0x2);

		const u8 paletteNum =
			((m_BGPaletteLowShift >> bitIndex) & 1) |
			((m_BGPaletteHighShift >> (bitIndex - 1)) & 0x2);

		backgroundPixels = (paletteNum << 2) | paletteOffset;
	}

	u8 spritePixels = 0;
	if ((m_MaskReg & MASK_SPRITE_ENABLE_BIT) && 
		(x >= 8 || m_MaskReg & MASK_SPRITE_LEFT_COLUMN_ENABLE_BIT))
	{
		spritePixels = m_SpritePixelBuf[x].color;
	}

	u8 paletteAddr = 0;
	const bool opaqueSprite = spritePixels & 0b11;
	const bool opaqueBackground = backgroundPixels & 0b11;


	// Sprite 0 hit detection
	if (opaqueSprite && opaqueBackground && m_SpritePixelBuf[x].sprite0Flag)
	{
		m_StatusReg |= STATUS_SPRITE0_HIT_BIT;
	}

	// both transparent, choose backdrop

	if (!opaqueSprite && !opaqueBackground)
	{
		paletteAddr = 0;
	}
	else if (!opaqueBackground || (opaqueSprite && !m_SpritePixelBuf[x].priority))
	{
		paletteAddr = (1 << 4) | spritePixels;
	}
	else
	{
		paletteAddr = backgroundPixels;
	}

	m_Framebuffer[y * SCREEN_WIDTH + x] = PaletteRead(paletteAddr);
}

void PPU::ShiftRegisters()
{
	if (!(m_MaskReg & MASK_BACKGROUND_ENABLE_BIT) || 
		(m_ScanlineCycle >= 257 && m_ScanlineCycle <= 320) || 
		m_ScanlineCycle >= 337)
	{
		return;
	}

	m_BGTileLowShift <<= 1;
	m_BGTileHighShift <<= 1;
	m_BGPaletteLowShift <<= 1;
	m_BGPaletteHighShift <<= 1;

	if (m_ScanlineCycle % 8 == 0)
	{
		const u8 coarseX = m_V & 0x1F;
		const u8 coarseY = (m_V >> 5) & 0x1F;
		const u8 shift = ((coarseY & 0x02) << 1) | (coarseX & 0x02);
		const u8 paletteBits = (m_AT >> shift) & 0b11;
		const u8 expandedLow = (paletteBits & 1) ? 0xFF : 0x00;
		const u8 expandedHigh = (paletteBits & 2) ? 0xFF : 0x00;

		m_BGTileLowShift = (m_BGTileLowShift & 0xFF00) | m_BGLow;
		m_BGTileHighShift = (m_BGTileHighShift & 0xFF00) | m_BGHigh;
		m_BGPaletteLowShift = (m_BGPaletteLowShift & 0xFF00) | expandedLow;
		m_BGPaletteHighShift = (m_BGPaletteHighShift & 0xFF00) | expandedHigh;
	}
}

bool PPU::RenderingEnabled() const
{
	return m_MaskReg & (MASK_BACKGROUND_ENABLE_BIT | MASK_SPRITE_ENABLE_BIT);
}

void PPU::FetchBackgroundData()
{
	// No fetch when background disabled
	if (!(m_MaskReg & MASK_BACKGROUND_ENABLE_BIT))
	{
		return;
	}

	switch (m_ScanlineCycle % 8)
	{
	// NT fetch
	case 1:
		m_NT = NametableRead(m_V & 0x0FFF);
		break;
		// AT fetch
	case 3:
		m_AT = NametableRead(
			0x3C0 | (m_V & 0x0C00) | ((m_V >> 4) & 0x38) | ((m_V >> 2) & 0x07)
		);
		break;
	// BG lsbits fetch
	case 5: {
		const u16 fineY = (m_V & INTERNAL_FINE_Y_SCROLL_MASK) >> INTERNAL_FINE_Y_SCROLL_SHIFT;
		const u16 tileNumber = m_NT;
		const u16 patternHalf = (m_CtrlReg & CTRL_BACKGROUND_TILE_SELECT_BIT) >> CTRL_BACKGROUND_TILE_SELECT_SHIFT;
		const u16 addr =
			fineY |
			(tileNumber << PATTERN_TABLE_TILE_SHIFT) |
			(patternHalf << PATTERN_TABLE_HALF_SHIFT);
		m_BGLow = m_Mapper->PpuRead(addr);
		break;
	}
	// BG msbits fetch
	case 7: {
		const u16 fineY = (m_V & INTERNAL_FINE_Y_SCROLL_MASK) >> INTERNAL_FINE_Y_SCROLL_SHIFT;
		const u16 tileNumber = m_NT;
		const u16 patternHalf = (m_CtrlReg & CTRL_BACKGROUND_TILE_SELECT_BIT) >> CTRL_BACKGROUND_TILE_SELECT_SHIFT;
		const u16 addr =
			fineY |
			(1 << PATTERN_TABLE_BIT_PLANE_SHIFT) | // read from right bitplane instead of left
			(tileNumber << PATTERN_TABLE_TILE_SHIFT) |
			(patternHalf << PATTERN_TABLE_HALF_SHIFT);
		m_BGHigh = m_Mapper->PpuRead(addr);
		break;
	}
	default:
		break;
	}
}

void PPU::EvaluateSprites()
{
	// TODO: check if this should still happen when sprite rendering disabled
	if (!(m_MaskReg & MASK_SPRITE_ENABLE_BIT))
	{
		return;
	}

	if (m_ScanlineCycle == 1)
	{
		// technically each byte written should take 2 cycles, but it has no impact
		// since secondary oam is never read from outside the ppu
		m_SecondaryOam.fill(0xFF);
	}
	// TODO: Right now we're performing this all at once, but
	// the real PPU perform this from cycles 65 to 256
	// Might have an effect for certain ROMS, idk
	else if (m_ScanlineCycle == 65)
	{
		FillSecondaryOAM();
	}
	else if (m_ScanlineCycle == 257)
	{
		FetchSpriteData();
	}
}
 
void PPU::FillSecondaryOAM()
{
	m_Sprite0InRange = false;
	u8 spritesFound = 0;
	u8 n;
	for (n = 0; n < 64; n++)
	{
		const u8 yPos = m_Oam[n * 4];
		if (SpriteInRange(yPos))
		{
			if (n == 0)
			{
				m_Sprite0InRange = true;
			}
			std::memcpy(
				m_SecondaryOam.data() + spritesFound * 4,
				m_Oam.data() + n * 4,
				4);
			spritesFound++;
			if (spritesFound == 8)
			{
				break;
			}
		}
	}
	u8 m = 0;
	for (n = n + 1; n < 64; n++)
	{
		const u8 yPos = m_Oam[n * 4 + m];
		if (SpriteInRange(yPos))
		{
			m_StatusReg |= STATUS_SPRITE_OVERFLOW_BIT;
			break;
		}
		m = (m + 1) % 4;
	}
}

void PPU::FetchSpriteData()
{
	m_SpritePixelBuf.fill({});
	const bool largeSprite = m_CtrlReg & CTRL_SPRITE_SIZE_BIT;

	for (u8 i = 0; i < m_SecondaryOam.size() && m_SecondaryOam[i] != 0xFF; i += 4)
	{
		const Sprite sprite = *reinterpret_cast<Sprite*>(&m_SecondaryOam[i]);

		// For 8x8 sprites
		u8 tileNum = sprite.tileIndex;
		u8 patternHalf = (m_CtrlReg >> CTRL_SPRITE_TILE_SELECT_SHIFT) & 1;
		u8 fineY = m_Scanline - sprite.yPos;

		ASSERT(fineY <= 15);

		const bool flipHorizontal = sprite.attributes & SPRITE_ATTRIBUTE_FLIP_HORIZONTAL_BIT;
		const bool flipVertical = sprite.attributes & SPRITE_ATTRIBUTE_FLIP_VERTICAL_BIT;
		if (flipVertical)
		{
			if (largeSprite)
			{
				fineY = 15 - fineY;
			}
			else
			{
				fineY = 7 - fineY;
			}
		}

		// For 8x16 sprites
		if (largeSprite)
		{
			patternHalf = sprite.tileIndex & 1;
			tileNum = sprite.tileIndex & 0xFE;
			// For 8x16 sprites, read the next tile after if fineY >= 8
			if (fineY >= 8)
			{
				fineY -= 8;
				tileNum++;
			}
		}

		const u16 addrLow =
			fineY |
			(tileNum << PATTERN_TABLE_TILE_SHIFT) |
			(patternHalf << PATTERN_TABLE_HALF_SHIFT);

		const u16 addrHigh =
			addrLow | (1 << PATTERN_TABLE_BIT_PLANE_SHIFT);

		const u8 patternLow = m_Mapper->PpuRead(addrLow);
		const u8 patternHigh = m_Mapper->PpuRead(addrHigh);

		const u8 paletteBits = sprite.attributes & SPRITE_ATTRIBUTE_PALETTE_MASK;
		const u8 priority = (sprite.attributes >> SPRITE_ATTRIBUTE_PRIORITY_SHIFT) & 1;

		for (u8 dx = 0; dx < 8; dx++)
		{
			const u8 fineX = flipHorizontal ? 7 - dx : dx;
			const u8 bitIndex = 7 - fineX;
			const u16 lineX = sprite.xPos + dx;
			if (lineX >= SCREEN_WIDTH)
			{
				break;
			}
			const u8 color =
				((patternLow >> bitIndex) & 1) |
				(((patternHigh >> bitIndex) & 1) << 1) |
				(paletteBits << 2);
			// take only first opaque pixels
			const bool sprite0Flag = m_Sprite0InRange && i == 0;
			if (!(m_SpritePixelBuf[lineX].color & 0b11) && (color & 0b11))
			{
				m_SpritePixelBuf[lineX] =
				{
					.color = color,
					.priority = priority,
					.sprite0Flag = sprite0Flag
				};
			}
		}
	}
}

bool PPU::SpriteInRange(u8 yPos) const
{
	// u16 to prevent wrapping around to make my life easier
	const u16 spriteStart = yPos;
	if (yPos >= SCREEN_HEIGHT - 1)
	{
		return false;
	}
	// determine height based on flag
	const u16 spriteEnd = spriteStart + ((m_CtrlReg & CTRL_SPRITE_SIZE_BIT) ? 15 : 7);
	return m_Scanline >= spriteStart && m_Scanline <= spriteEnd;
}

void PPU::UpdateV()
{
	if (!RenderingEnabled())
	{
		return;
	}

	// Skip on H blank
	if (m_ScanlineCycle % 8 == 0 && (m_ScanlineCycle <= 256 || m_ScanlineCycle >= 328))
	{
		IncHoriV();
	}

	if (m_ScanlineCycle == 256)
	{
		IncVertV();
	}

	if (m_ScanlineCycle == 257)
	{
		// hori(v) = hori(t)
		constexpr u16 mask =
			INTERNAL_COARSE_X_SCROLL_MASK |
			INTERNAL_NAMETABLE_X_MASK;
		m_V &= ~mask;
		m_V |= (m_T & mask);
	}
}

void PPU::IncHoriV()
{
	// Continue to next name table if coarse X == 31 (stolen from nesdev wiki)
	if ((m_V & INTERNAL_COARSE_X_SCROLL_MASK) == 31)
	{
		m_V = (m_V & ~INTERNAL_COARSE_X_SCROLL_MASK) ^ (1 << INTERNAL_NAMETABLE_SELECT_SHIFT);
	}
	// Otherwise increment coarse X normally
	else
	{
		m_V += 1;
	}
}

// Stolen from nesdev wiki
void PPU::IncVertV()
{
	if ((m_V & 0x7000) != 0x7000) // fine Y < 7
	{
		m_V += 0x1000;
	}
	else
	{
		m_V &= ~0x7000; // fine Y = 0
		u8 coarseY = (m_V & 0x03E0) >> 5;
		if (coarseY == 29)
		{
			coarseY = 0;
			m_V ^= 0x0800; // switch nametable
		}
		else if (coarseY == 31)
		{
			coarseY = 0;
		}
		else
		{
			coarseY++;
		}
		m_V = (m_V & ~0x03E0) | (coarseY << 5); // update coarse y
	}
}

void PPU::SetCtrl(u8 data)
{
	if (IgnoresWrites())
		return;

	const u8 old = m_CtrlReg;

	m_CtrlReg = data;
	m_T = (m_T & ~INTERNAL_NAMETABLE_SELECT_MASK) 
		| ((data & CTRL_NAMETABLE_MASK) << INTERNAL_NAMETABLE_SELECT_SHIFT);

	if ((data & CTRL_NMI_ENABLE_BIT) &&
		(m_StatusReg & STATUS_VBLANK_BIT))
	{
		m_Cpu->SetNMILine(true);
	}
	else if (!(data & CTRL_NMI_ENABLE_BIT))
	{
		m_Cpu->SetNMILine(false);
	}
}

void PPU::SetMask(u8 data)
{
	if (IgnoresWrites())
		return;

	m_MaskReg = data;
}

u8 PPU::GetStatus()
{
	m_W = 0;
	const u8 val = m_StatusReg;
	m_StatusReg &= ~STATUS_VBLANK_BIT;
	m_Cpu->SetNMILine(false);

	return val;
}

void PPU::SetOAMAddr(u8 data)
{
	m_OamAddr = data;

}

u8 PPU::GetOAMData()
{
	u8 val = m_Oam[m_OamAddr];

	// attribute byte (byte 2) has 0 read from unimplemented bits
	if ((m_OamAddr & 0x3) == 2)
		val &= ~SPRITE_ATTRIBUTE_UNIMPLEMENTED_MASK;

	return val;
}

void PPU::SetOAMData(u8 data)
{
	m_Oam[m_OamAddr] = data;
	m_OamAddr++;
}

void PPU::SetScroll(u8 data)
{
	if (IgnoresWrites())
		return;

	// x scroll
	if (!m_W)
	{
		m_X = data & SCROLL_FINE_MASK;
		m_T = (m_T & ~INTERNAL_COARSE_X_SCROLL_MASK) | (data >> 3);
	}
	// y scroll
	else
	{
		m_T = (m_T & ~INTERNAL_COARSE_Y_SCROLL_MASK) | ((data >> 3) << 5);
		m_T = (m_T & ~INTERNAL_FINE_Y_SCROLL_MASK) | ((data & SCROLL_FINE_MASK) << 12);
	}
	m_W ^= 1;
}

void PPU::SetAddr(u8 data)
{
	if (IgnoresWrites())
		return;

	// upper bits
	if (!m_W)
	{
		m_T = (m_T & 0xFF) | ((data & 0x3F) << 8);
	}
	// lower bits
	else
	{
		m_T = (m_T & 0xFF00) | data;
		m_V = m_T;
	}

	m_W ^= 1;
}

u8 PPU::GetData()
{
	const u8 read = Read(m_V);

	// reads from palette ram are not buffered
	const u8 ret = m_V >= 0x3F00 ? read : m_ReadBuf;

	m_ReadBuf = read;

	if (m_CtrlReg & CTRL_INCREMENT_MODE_BIT)
	{
		m_V += 32;
	}
	else
	{
		m_V += 1;
	}

	return ret;
}

void PPU::SetData(u8 data)
{
	Write(m_V, data);

	if (m_CtrlReg & CTRL_INCREMENT_MODE_BIT)
	{
		m_V += 32;
	}
	else
	{
		m_V += 1;
	}
}

void PPU::DirectOAMWrite(u8 data)
{
	m_Oam[m_OamAddr] = data;
	m_OamAddr++;
}


u8 PPU::Read(u16 addr)
{
	if (addr < 0x2000)
	{
		return m_Mapper->PpuRead(addr);
	}
	else if (addr < 0x3F00)
	{
		return NametableRead(addr - 0x2000);
	}
	else if (addr < 0x4000)
	{
		// if divisible by 4, mirror to 0x3F00
		return PaletteRead(addr & 0x1F);
	}
	return 0;
}

void PPU::Write(u16 addr, u8 val)
{
	if (addr < 0x2000)
	{
		m_Mapper->PpuWrite(addr, val);
		return;
	}
	else if (addr < 0x3F00)
	{
		NametableWrite(addr - 0x2000, val);
		return;
	}
	else if (addr < 0x4000)
	{
		// divisible by 4, mirror to 0x3F00
		PaletteWrite(addr & 0x1F, val);
		return;
	}
}

u8 PPU::NametableRead(u16 offset)
{
	const std::optional<u16> mirrored = m_Mapper->NametableMirror(offset);
	if (mirrored)
	{
		return m_Vram[*mirrored];
	}
	else
	{
		return m_Mapper->PpuRead(offset + 0x2000);
	}
}

void PPU::NametableWrite(u16 offset, u8 val)
{
	const std::optional<u16> mirrored = m_Mapper->NametableMirror(offset);
	if (mirrored)
	{
		m_Vram[*mirrored] = val;
	}
	else
	{
		m_Mapper->PpuWrite(offset + 0x2000, val);
	}
}

static constexpr u16 MirrorPalette(u16 offset)
{
	offset &= 0x1F;
	if ((offset & 0x13) == 0x10)
		offset &= ~0x10;
	return offset;
}

u8 PPU::PaletteRead(u16 offset)
{
	return m_Palette[MirrorPalette(offset)];
}

void PPU::PaletteWrite(u16 offset, u8 val)
{
	m_Palette[MirrorPalette(offset)] = val;
}

bool PPU::IgnoresWrites()
{
	//return false;
	return m_FrameNumber == 0 && m_Scanline < 261;
}