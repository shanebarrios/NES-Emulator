#pragma once

#include "../Core/Common.h"
#include "Cartridge.h"
#include <optional>

class Mapper
{
public:
	Mapper() = default;

	Mapper(Cartridge* cartridge) : m_Cartridge{ cartridge } {}

	void Attach(Cartridge* cartridge) { m_Cartridge = cartridge; }

	virtual ~Mapper() = default;

	virtual u8 CpuRead(u16 addr) = 0;

	virtual void CpuWrite(u16 addr, u8 data) = 0;

	virtual u8 PpuRead(u16 addr) = 0;

	virtual void PpuWrite(u16 addr, u8 data) = 0;

	// Takes in nametable offset, returns mirrored VRAM addr 
	// or std::nullopt if mapper redirects to cartridge
	virtual std::optional<u16> NametableMirror(u16 offset);

	virtual MirrorMode GetMirrorMode() const { return m_Cartridge->GetMirrorMode(); }

protected:
	Cartridge* m_Cartridge = nullptr;
};

class NROM : public Mapper
{
public:
	NROM() : Mapper{} {}

	NROM(Cartridge* cartridge) : Mapper{ cartridge } {}

	u8 CpuRead(u16 addr) override;

	void CpuWrite(u16 addr, u8 data) override;

	u8 PpuRead(u16 addr) override;

	void PpuWrite(u16 addr, u8 data) override;
};