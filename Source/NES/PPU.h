#pragma once

#include "../Core/Common.h"
#include "Mapper.h"
#include <bitset>

class Cartridge;
class CPU;

struct Sprite
{
	u8 yPos = 0;
	u8 tileIndex = 0;
	u8 attributes = 0;
	u8 xPos = 0;
};

enum class SpriteEvalPhase
{
	Phase1,
	Phase2,
	Phase3,
	Phase4
};

class PPU
{
public:
	static constexpr u16 SCREEN_WIDTH = 256;
	static constexpr u16 SCREEN_HEIGHT = 240;

	PPU();

	PPU(CPU* cpu, Mapper* mapper);

	void Attach(CPU* cpu, Mapper* mapper);

	void Reset();

	void PerformCycle();

	bool FramebufferReady() const { return m_FramebufferReady; }

	void ClearFramebufferReady() { m_FramebufferReady = false; }

	const u8* GetFramebuffer() const { return m_Framebuffer.get(); }

	void SetCtrl(u8 data);

	void SetMask(u8 data);

	u8 GetStatus();

	void SetOAMAddr(u8 data);

	u8 GetOAMData();

	void SetOAMData(u8 data);

	void SetScroll(u8 data);

	void SetAddr(u8 data);

	u8 GetData();

	void SetData(u8 data);

	void DMA(const Memory<256>& mem);

private:
	u8 Read(u16 addr);

	void Write(u16 addr, u8 val);

	u8 NametableRead(u16 offset);

	void NametableWrite(u16 offset, u8 val);

	u8 PaletteRead(u16 offset);

	void PaletteWrite(u16 offset, u8 val);

	void PrerenderCycle();

	void VBlankCycle();

	void VisibleCycle();

	void FetchBackgroundData();

	void EvaluateSprites();

	void FillSecondaryOAM();

	void FetchSpriteData();

	bool SpriteInRange(u8 yPos) const;

	void UpdateV();

	bool IgnoresWrites();

	void AdvanceCycle();

	void SetPixel();

	void ShiftRegisters();

	void IncHoriV();

	void IncVertV();

	bool RenderingEnabled() const;

private:
	Memory<0x800> m_Vram{};
	Memory<0x20> m_Palette{};
	Memory<0x100> m_Oam{};
	Memory<0x20> m_SecondaryOam{};
	std::unique_ptr<u8[]> m_Framebuffer = nullptr;

	CPU* m_Cpu = nullptr;
	Mapper* m_Mapper = nullptr;

	// I/O registers
	u8 m_CtrlReg = 0;
	u8 m_MaskReg = 0;
	u8 m_StatusReg = 0;
	u8 m_OamAddr = 0;
	u16 m_Scroll = 0;

	// Internal registers
	u16 m_V = 0;
	u16 m_T = 0;
	u8 m_X = 0;
	u8 m_W = 0;

	// Fetch registers
	u8 m_NT = 0; 
	u8 m_AT = 0;
	u8 m_BGLow = 0;
	u8 m_BGHigh = 0;

	// Shift registers
	u16 m_BGTileLowShift = 0;
	u16 m_BGTileHighShift = 0;
	u16 m_BGPaletteLowShift = 0;
	u16 m_BGPaletteHighShift = 0;

	// Sprite evaluation
	u8 m_Sprite0NextStart = 0;
	Memory<SCREEN_WIDTH> m_SpritePixelBuf{};
	std::bitset<SCREEN_WIDTH> m_SpritePriorityBuf{};

	u8 m_ReadBuf = 0;

	u16 m_Scanline = 0;
	u16 m_ScanlineCycle = 0;
	u64 m_FrameNumber = 0;

	bool m_FramebufferReady = false;
};