#pragma once

#include "Common.h"

class CPU;
class Cartridge;

class Bus
{
public:
	Bus() = default;
	Bus(CPU* _cpu, Cartridge* cartridge, uint8_t* ram) :
		m_CPU{ _cpu }, m_Cartridge{ cartridge }, m_RAM{ ram } {}

	void Attach(CPU* _cpu, Cartridge* cartridge, uint8_t* ram);

	uint8_t Read(uint16_t addr) const;
	void Write(uint16_t addr, uint8_t val);

private:
	CPU* m_CPU = nullptr;
	Cartridge* m_Cartridge = nullptr;
	u8* m_RAM = nullptr;
};