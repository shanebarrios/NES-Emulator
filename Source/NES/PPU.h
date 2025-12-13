#pragma once

#include <array>
#include "../Core/Common.h"
#include "Mapper.h"
#include "SystemCommon.h"

class Cartridge;
class CPU;
struct RGB8;

inline constexpr u16 SCREEN_WIDTH = 256;
inline constexpr u16 SCREEN_HEIGHT = 240;

using PPUFramebuffer = Array2D<RGB8, SCREEN_WIDTH, SCREEN_HEIGHT>;

class PPU
{
public:
	PPU();

	PPU(CPU* cpu, Mapper* mapper, const RGB8* systemPalette);

	void Attach(CPU* cpu, Mapper* mapper, const RGB8* systemPalette);

	void Reset();

	void PerformCycle();

	bool FramebufferReady() const { return m_FramebufferReady; }

	void ClearFramebufferReady() { m_FramebufferReady = false; }

	const PPUFramebuffer& GetFramebuffer() const { return m_Framebuffer; }

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

	void PrerenderCycle();

	void VBlankCycle();

	void VisibleCycle();

	void FetchAndUpdate();

	bool IgnoresWrites();

private:
	Memory<0x800> m_Vram{};
	Memory<0x100> m_Oam{};
	Memory<0x20> m_Palette{};
	Memory<0x20> m_SecondaryOam{};

	CPU* m_Cpu = nullptr;
	Mapper* m_Mapper = nullptr;
	const RGB8* m_SystemPalette = nullptr;

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

	u8 m_ReadBuf = 0;

	u16 m_Scanline = 0;
	u16 m_ScanlineCycle = 0;
	u64 m_FrameNumber = 0;

	bool m_FramebufferReady = false;

	PPUFramebuffer m_Framebuffer{};
};