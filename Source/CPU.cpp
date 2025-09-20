#include "CPU.h"

#include "Bus.h"
#include "Common.h"

static constexpr uint8_t INTERRUPT_DISABLE_DELAY_NONE = 0x0;
static constexpr uint8_t INTERRUPT_DISABLE_DELAY_OFF = 0x1;
static constexpr uint8_t INTERRUPT_DISABLE_DELAY_ON = 0x3;

static constexpr uint16_t STACK_BEGIN = 0x0100;


const std::array<Instruction, 256> CPU::s_OpcodeLookup = [] {
	std::array<Instruction, 256> lookup{};

	lookup[0x69] = { &CPU::ADC, AddrMode::Immediate, 2, 2 };
	lookup[0x65] = { &CPU::ADC, AddrMode::ZeroPage, 2, 3 };
	lookup[0x75] = { &CPU::ADC, AddrMode::ZeroPageX, 2, 4 };
	lookup[0x6D] = { &CPU::ADC, AddrMode::Absolute, 3, 4 };
	lookup[0x7D] = { &CPU::ADC, AddrMode::AbsoluteX, 3, 4 };
	lookup[0x79] = { &CPU::ADC, AddrMode::AbsoluteY, 3, 4 };
	lookup[0x61] = { &CPU::ADC, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0x71] = { &CPU::ADC, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0x29] = { &CPU::AND, AddrMode::Immediate, 2, 2 };
	lookup[0x25] = { &CPU::AND, AddrMode::ZeroPage, 2, 3 };
	lookup[0x35] = { &CPU::AND, AddrMode::ZeroPageX, 2, 4 };
	lookup[0x2D] = { &CPU::AND, AddrMode::Absolute, 3, 4 };
	lookup[0x3D] = { &CPU::AND, AddrMode::AbsoluteX, 3, 4 };
	lookup[0x39] = { &CPU::AND, AddrMode::AbsoluteY, 3, 4 };
	lookup[0x21] = { &CPU::AND, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0x31] = { &CPU::AND, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0x0A] = { &CPU::ASL, AddrMode::Accumulator, 1, 2 };
	lookup[0x06] = { &CPU::ASL, AddrMode::ZeroPage, 2, 5 };
	lookup[0x16] = { &CPU::ASL, AddrMode::ZeroPageX, 2, 6 };
	lookup[0x0E] = { &CPU::ASL, AddrMode::Absolute, 3, 6 };
	lookup[0x1E] = { &CPU::ASL, AddrMode::AbsoluteX, 3, 7 };

	lookup[0x90] = { &CPU::BCC, AddrMode::Relative, 2, 2 };

	lookup[0xB0] = { &CPU::BCS, AddrMode::Relative, 2, 2 };

	lookup[0xF0] = { &CPU::BEQ, AddrMode::Relative, 2, 2 };

	lookup[0x24] = { &CPU::BIT, AddrMode::ZeroPage, 2, 3 };
	lookup[0x2C] = { &CPU::BIT, AddrMode::Absolute, 3, 4 };

	lookup[0x30] = { &CPU::BMI, AddrMode::Relative, 2, 2 };

	lookup[0xD0] = { &CPU::BNE, AddrMode::Relative, 2, 2 };

	lookup[0x10] = { &CPU::BPL, AddrMode::Relative, 2, 2 };

	lookup[0x00] = { &CPU::BRK, AddrMode::Implicit, 1, 7 };

	lookup[0x50] = { &CPU::BVC, AddrMode::Relative, 2, 2 };

	lookup[0x70] = { &CPU::BVS, AddrMode::Relative, 2, 2 };

	lookup[0x18] = { &CPU::CLC, AddrMode::Implicit, 1, 2 };

	lookup[0xD8] = { &CPU::CLD, AddrMode::Implicit, 1, 2 };

	lookup[0x58] = { &CPU::CLI, AddrMode::Implicit, 1, 2 };

	lookup[0xB8] = { &CPU::CLV, AddrMode::Implicit, 1, 2 };

	lookup[0xC9] = { &CPU::CMP, AddrMode::Immediate, 2, 2 };
	lookup[0xC5] = { &CPU::CMP, AddrMode::ZeroPage, 2, 3 };
	lookup[0xD5] = { &CPU::CMP, AddrMode::ZeroPageX, 2, 4 };
	lookup[0xCD] = { &CPU::CMP, AddrMode::Absolute, 3, 4 };
	lookup[0xDD] = { &CPU::CMP, AddrMode::AbsoluteX, 3, 4 };
	lookup[0xD9] = { &CPU::CMP, AddrMode::AbsoluteY, 3, 4 };
	lookup[0xC1] = { &CPU::CMP, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0xD1] = { &CPU::CMP, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0xE0] = { &CPU::CPX, AddrMode::Immediate, 2, 2 };
	lookup[0xE4] = { &CPU::CPX, AddrMode::ZeroPage, 2, 3 };
	lookup[0xEC] = { &CPU::CPX, AddrMode::Absolute, 3, 4 };

	lookup[0xC0] = { &CPU::CPY, AddrMode::Immediate, 2, 2 };
	lookup[0xC4] = { &CPU::CPY, AddrMode::ZeroPage, 2, 3 };
	lookup[0xCC] = { &CPU::CPY, AddrMode::Absolute, 3, 4 };

	lookup[0xC6] = { &CPU::DEC, AddrMode::ZeroPage, 2, 5 };
	lookup[0xD6] = { &CPU::DEC, AddrMode::ZeroPageX, 2, 6 };
	lookup[0xCE] = { &CPU::DEC, AddrMode::Absolute, 3, 6 };
	lookup[0xDE] = { &CPU::DEC, AddrMode::AbsoluteX, 3, 7 };

	lookup[0xCA] = { &CPU::DEX, AddrMode::Implicit, 1, 2 };

	lookup[0x88] = { &CPU::DEX, AddrMode::Implicit, 1, 2 };

	lookup[0x49] = { &CPU::EOR, AddrMode::Immediate, 2, 2 };
	lookup[0x45] = { &CPU::EOR, AddrMode::ZeroPage, 2, 3 };
	lookup[0x55] = { &CPU::EOR, AddrMode::ZeroPageX, 2, 4 };
	lookup[0x4D] = { &CPU::EOR, AddrMode::Absolute, 3, 4 };
	lookup[0x5D] = { &CPU::EOR, AddrMode::AbsoluteX, 3, 4 };
	lookup[0x59] = { &CPU::EOR, AddrMode::AbsoluteY, 3, 4 };
	lookup[0x41] = { &CPU::EOR, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0x51] = { &CPU::EOR, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0xE6] = { &CPU::INC, AddrMode::ZeroPage, 2, 5 };
	lookup[0xF6] = { &CPU::INC, AddrMode::ZeroPageX, 2, 6 };
	lookup[0xEE] = { &CPU::INC, AddrMode::Absolute, 3, 6 };
	lookup[0xFE] = { &CPU::INC, AddrMode::AbsoluteX, 3, 7 };

	lookup[0xE8] = { &CPU::INX, AddrMode::Implicit, 1, 2 };

	lookup[0xC8] = { &CPU::INY, AddrMode::Implicit, 1, 2 };

	lookup[0x4C] = { &CPU::JMP, AddrMode::Absolute, 3, 3 };
	lookup[0x6C] = { &CPU::JMP, AddrMode::Indirect, 3, 5 };

	lookup[0x20] = { &CPU::JSR, AddrMode::Absolute, 3, 6 };

	lookup[0xA9] = { &CPU::LDA, AddrMode::Immediate, 2, 2 };
	lookup[0xA5] = { &CPU::LDA, AddrMode::ZeroPage, 2, 3 };
	lookup[0xB5] = { &CPU::LDA, AddrMode::ZeroPageX, 2, 4 };
	lookup[0xAD] = { &CPU::LDA, AddrMode::Absolute, 3, 4 };
	lookup[0xBD] = { &CPU::LDA, AddrMode::AbsoluteX, 3, 4 };
	lookup[0xB9] = { &CPU::LDA, AddrMode::AbsoluteY, 3, 4 };
	lookup[0xA1] = { &CPU::LDA, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0xB1] = { &CPU::LDA, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0xA2] = { &CPU::LDX, AddrMode::Immediate, 2, 2 };
	lookup[0xA6] = { &CPU::LDX, AddrMode::ZeroPage, 2, 3 };
	lookup[0xB6] = { &CPU::LDX, AddrMode::ZeroPageY, 2, 4 };
	lookup[0xAE] = { &CPU::LDX, AddrMode::Absolute, 3, 4 };
	lookup[0xBE] = { &CPU::LDX, AddrMode::AbsoluteY, 3, 4 };

	lookup[0xA0] = { &CPU::LDY, AddrMode::Immediate, 2, 2 };
	lookup[0xA4] = { &CPU::LDY, AddrMode::ZeroPage, 2, 3 };
	lookup[0xB4] = { &CPU::LDY, AddrMode::ZeroPageX, 2, 4 };
	lookup[0xAC] = { &CPU::LDY, AddrMode::Absolute, 3, 4 };
	lookup[0xBC] = { &CPU::LDY, AddrMode::AbsoluteX, 3, 4 };

	lookup[0x4A] = { &CPU::LSR, AddrMode::Accumulator, 1, 2 };
	lookup[0x46] = { &CPU::LSR, AddrMode::ZeroPage, 2, 5 };
	lookup[0x56] = { &CPU::LSR, AddrMode::ZeroPageX, 2, 6 };
	lookup[0x4E] = { &CPU::LSR, AddrMode::Absolute, 3, 6 };
	lookup[0x5E] = { &CPU::LSR, AddrMode::AbsoluteX, 3, 7 };

	lookup[0xEA] = { &CPU::NOP, AddrMode::Implicit, 1, 2 };

	lookup[0x09] = { &CPU::ORA, AddrMode::Implicit, 2, 2 };
	lookup[0x05] = { &CPU::ORA, AddrMode::ZeroPage, 2, 3 };
	lookup[0x15] = { &CPU::ORA, AddrMode::ZeroPageX, 2, 4 };
	lookup[0x0D] = { &CPU::ORA, AddrMode::Absolute, 3, 4 };
	lookup[0x1D] = { &CPU::ORA, AddrMode::AbsoluteX, 3, 4 };
	lookup[0x19] = { &CPU::ORA, AddrMode::AbsoluteY, 3, 4 };
	lookup[0x01] = { &CPU::ORA, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0x11] = { &CPU::ORA, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0x48] = { &CPU::PHA, AddrMode::Implicit, 1, 3 };

	lookup[0x08] = { &CPU::PHP, AddrMode::Implicit, 1, 3 };

	lookup[0x68] = { &CPU::PLA, AddrMode::Implicit, 1, 4 };

	lookup[0x28] = { &CPU::PLP, AddrMode::Implicit, 1, 4 };

	lookup[0x2A] = { &CPU::ROL, AddrMode::Accumulator, 1, 2 };
	lookup[0x26] = { &CPU::ROL, AddrMode::ZeroPage, 2, 5 };
	lookup[0x36] = { &CPU::ROL, AddrMode::ZeroPageX, 2, 6 };
	lookup[0x2E] = { &CPU::ROL, AddrMode::Absolute, 3, 6 };
	lookup[0x3E] = { &CPU::ROL, AddrMode::AbsoluteX, 3, 7 };

	lookup[0x6A] = { &CPU::ROR, AddrMode::Accumulator, 1, 2 };
	lookup[0x66] = { &CPU::ROR, AddrMode::ZeroPage, 2, 5 };
	lookup[0x76] = { &CPU::ROR, AddrMode::ZeroPageX, 2, 6 };
	lookup[0x6E] = { &CPU::ROR, AddrMode::Absolute, 3, 6 };
	lookup[0x7E] = { &CPU::ROR, AddrMode::AbsoluteX, 3, 7 };

	lookup[0x40] = { &CPU::RTI, AddrMode::Implicit, 1, 6 };

	lookup[0x60] = { &CPU::RTS, AddrMode::Implicit, 1, 6 };

	lookup[0xE9] = { &CPU::SBC, AddrMode::Immediate, 2, 2 };
	lookup[0xE5] = { &CPU::SBC, AddrMode::ZeroPage, 2, 3 };
	lookup[0xF5] = { &CPU::SBC, AddrMode::ZeroPageX, 2, 4 };
	lookup[0xED] = { &CPU::SBC, AddrMode::Absolute, 3, 4 };
	lookup[0xFD] = { &CPU::SBC, AddrMode::AbsoluteX, 3, 4 };
	lookup[0xF9] = { &CPU::SBC, AddrMode::AbsoluteY, 3, 4 };
	lookup[0xE1] = { &CPU::SBC, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0xF1] = { &CPU::SBC, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0x38] = { &CPU::SEC, AddrMode::Implicit, 1, 2 };

	lookup[0xF8] = { &CPU::SED, AddrMode::Implicit, 1, 2 };

	lookup[0x78] = { &CPU::SEI, AddrMode::Implicit, 1, 2 };

	lookup[0x85] = { &CPU::STA, AddrMode::ZeroPage, 2, 3 };
	lookup[0x95] = { &CPU::STA, AddrMode::ZeroPageX, 2, 4 };
	lookup[0x8D] = { &CPU::STA, AddrMode::Absolute, 3, 4 };
	lookup[0x9D] = { &CPU::STA, AddrMode::AbsoluteX, 3, 5 };
	lookup[0x99] = { &CPU::STA, AddrMode::AbsoluteY, 3, 5 };
	lookup[0x81] = { &CPU::STA, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0x91] = { &CPU::STA, AddrMode::IndirectIndexed, 2, 6 };

	lookup[0x86] = { &CPU::STX, AddrMode::ZeroPage, 2, 3 };
	lookup[0x96] = { &CPU::STX, AddrMode::ZeroPageY, 2, 4 };
	lookup[0x8E] = { &CPU::STX, AddrMode::Absolute, 3, 4 };

	lookup[0x84] = { &CPU::STY, AddrMode::ZeroPage, 2, 3 };
	lookup[0x94] = { &CPU::STY, AddrMode::ZeroPageX, 2, 4 };
	lookup[0x8C] = { &CPU::STY, AddrMode::Absolute, 3, 4 };

	lookup[0xAA] = { &CPU::TAX, AddrMode::Implicit, 1, 2 };

	lookup[0xA8] = { &CPU::TAY, AddrMode::Implicit, 1, 2 };

	lookup[0xBA] = { &CPU::TSX, AddrMode::Implicit, 1, 2 };

	lookup[0x8A] = { &CPU::TXA, AddrMode::Implicit, 1, 2 };

	lookup[0x9A] = { &CPU::TXS, AddrMode::Implicit, 1, 2 };

	lookup[0x98] = { &CPU::TYA, AddrMode::Implicit, 1, 2 };

	return lookup;
}();

CPU::CPU(Bus& bus) : m_Bus{ bus }
{
	m_Regs.A = 0;
	m_Regs.X = 0;
	m_Regs.Y = 0;
	//m_Regs.PC = ReadWord(0xFFFC);
	m_Regs.PC = 0xC000;
	m_Regs.S = 0xFD;
	m_StatusReg = StatusFlag::InterruptDisable;
}

void CPU::PerformCycle()
{
	if (m_InstructionRemainingCycles > 0)
	{
		m_InstructionRemainingCycles--;
		return;
	}

	if (m_InterruptDisableDelay != INTERRUPT_DISABLE_DELAY_NONE)
	{
		m_StatusReg.Set(StatusFlag::InterruptDisable, m_InterruptDisableDelay == INTERRUPT_DISABLE_DELAY_ON);
	}

	const uint8_t opCode = Read(m_Regs.PC);

	const Instruction& instruction = s_OpcodeLookup[opCode];

	const uint8_t addCycles = (this->*instruction.op)(instruction.addrMode);
	m_InstructionRemainingCycles = addCycles + instruction.cycleCount;
	if (!m_PCManuallySet)
		m_Regs.PC += instruction.byteCount;

	if (Read(m_Regs.PC) == 0x04)
		__debugbreak();

	m_InterruptDisableDelay = INTERRUPT_DISABLE_DELAY_NONE;
	m_PCManuallySet = false;
}

uint8_t CPU::Read(uint16_t addr) const
{
	return m_Bus.Read(addr);
}

uint16_t CPU::ReadWord(uint16_t addr) const
{
	const uint8_t low = m_Bus.Read(addr);
	const uint8_t high = m_Bus.Read(addr + 1);
	return low | (high << 8);
}

void CPU::Write(uint16_t addr, uint8_t val)
{
	return m_Bus.Write(addr, val);
}

void CPU::WriteWord(uint16_t addr, uint16_t val)
{
	const uint8_t low = val & 0xFF;
	const uint8_t high = (val >> 8) & 0xFF;
	m_Bus.Write(addr, low);
	m_Bus.Write(addr + 1, high);
}

void CPU::PushStack(uint8_t val)
{
	Write(m_Regs.S + STACK_BEGIN, val);
	ASSERT_MSG(m_Regs.S != 0, "Stack overflow!");
	m_Regs.S--;
}

void CPU::PushStackWord(uint16_t val)
{
	const uint8_t low = val & 0xFF;
	const uint8_t high = (val >> 8) & 0xFF;
	PushStack(high);
	PushStack(low);
}

uint8_t CPU::PopStack()
{
	m_Regs.S++;
	return Read(m_Regs.S + STACK_BEGIN);
}

uint16_t CPU::PopStackWord()
{
	const uint8_t low = PopStack();
	const uint8_t high = PopStack();
	return low | (high << 8);
}

uint8_t CPU::PeekStack() const
{
	return Read(m_Regs.S + 1 + STACK_BEGIN);
}

uint16_t CPU::PeekStackWord() const
{
	const uint8_t low = Read(m_Regs.S + 1 + STACK_BEGIN);
	const uint8_t high = Read(m_Regs.S + 2 + STACK_BEGIN);
	return low | (high << 8);
}

bool CPU::AddsCycle(AddrMode addrMode) const
{
	switch (addrMode)
	{
	case AddrMode::AbsoluteX:
	{
		const uint16_t base = ReadWord(m_Regs.PC + 1);
		const uint16_t addr = base + m_Regs.X;
		return (base & 0xFF00) != (addr & 0xFF00);
	}
	case AddrMode::AbsoluteY:
	{
		const uint16_t base = ReadWord(m_Regs.PC + 1);
		const uint16_t addr = base + m_Regs.Y;
		return (base & 0xFF00) != (addr & 0xFF00);
	}

	case AddrMode::IndirectIndexed: 
	{
		const uint8_t arg = Read(m_Regs.PC + 1);
		const uint8_t low = Read(arg);
		const uint8_t high = Read((arg + 1) & 0xFF);
		const uint16_t base = low | (high << 8);
		const uint16_t addr = base + m_Regs.Y;
		return (base & 0xFF00) != (addr & 0xFF00);
	}
	default:
		return false;
	}
}

uint8_t CPU::Branch(uint16_t offset)
{
	const uint16_t updated = m_Regs.PC + 2 + offset;
	uint8_t addCycles = 1;
	if ((updated & 0xFF00) != (m_Regs.PC & 0xFF00))
		addCycles++;
	m_Regs.PC = updated;
	m_PCManuallySet = true;
	return addCycles;
}

uint16_t CPU::ResolveAddress(AddrMode addrMode) const
{
	switch (addrMode)
	{
	case AddrMode::Implicit:
		return ResolveImplicit();
	case AddrMode::Accumulator:
		return ResolveAccumulator();
	case AddrMode::Immediate:
		return ResolveImmediate();
	case AddrMode::ZeroPage:
		return ResolveZeroPage();
	case AddrMode::ZeroPageX:
		return ResolveZeroPageX();
	case AddrMode::ZeroPageY:
		return ResolveZeroPageY();
	case AddrMode::Relative:
		return ResolveRelative();
	case AddrMode::Absolute:
		return ResolveAbsolute();
	case AddrMode::AbsoluteX:
		return ResolveAbsoluteX();
	case AddrMode::AbsoluteY:
		return ResolveAbsoluteY();
	case AddrMode::Indirect:
		return ResolveIndirect();
	case AddrMode::IndexedIndirect:
		return ResolveIndexedIndirect();
	case AddrMode::IndirectIndexed:
		return ResolveIndirectIndexed();
	default:
		return 0;
	}
}

uint16_t CPU::ResolveImplicit() const
{
	return 0;
}

uint16_t CPU::ResolveAccumulator() const
{
	return 0;
}

uint16_t CPU::ResolveImmediate() const
{
	return m_Regs.PC + 1;
}

uint16_t CPU::ResolveZeroPage() const
{
	return Read(m_Regs.PC + 1);
}

uint16_t CPU::ResolveZeroPageX() const
{
	return (Read(m_Regs.PC + 1) + m_Regs.X) & 0xFF;
}

uint16_t CPU::ResolveZeroPageY() const
{
	return (Read(m_Regs.PC + 1) + m_Regs.Y) & 0xFF;
}

uint16_t CPU::ResolveRelative() const
{
	const uint8_t addr = Read(m_Regs.PC + 1);
	if (addr & 0x80)
	{
		return addr | 0xFF00;
	}
	return addr;
}

uint16_t CPU::ResolveAbsolute() const
{
	return ReadWord(m_Regs.PC + 1);
}

uint16_t CPU::ResolveAbsoluteX() const
{
	return ReadWord(m_Regs.PC + 1) + m_Regs.X;
}

uint16_t CPU::ResolveAbsoluteY() const
{
	return ReadWord(m_Regs.PC + 1) + m_Regs.Y;
}

uint16_t CPU::ResolveIndirect() const
{
	const uint16_t addr = ReadWord(m_Regs.PC + 1);
	const uint8_t low = Read(addr);
	// Intentional bug
	const uint8_t high = ((addr & 0xFF) == 0xFF) ? Read(addr & 0xFF00) : Read(addr + 1);
	return low | (high << 8);
}

uint16_t CPU::ResolveIndexedIndirect() const
{
	const uint8_t arg = Read(m_Regs.PC + 1);
	const uint8_t low = Read((arg + m_Regs.X) & 0xFF);
	const uint8_t high = Read((arg + m_Regs.X + 1) & 0xFF);
	return low | (high << 8);
}

uint16_t CPU::ResolveIndirectIndexed() const
{
	const uint8_t arg = Read(m_Regs.PC + 1);
	const uint8_t low = Read(arg);
	const uint8_t high = Read((arg + 1) & 0xFF);
	const uint16_t baseAddr = low | (high << 8);
	return baseAddr + m_Regs.Y;
}

uint8_t CPU::ADC(AddrMode addrMode) 
{
	const uint8_t val = Read(ResolveAddress(addrMode));
	const uint16_t result = m_Regs.A + val + m_StatusReg.Test(StatusFlag::Carry);

	m_StatusReg.Set(StatusFlag::Carry, result > 0xFF);
	m_StatusReg.Set(StatusFlag::Zero, result == 0);
	m_StatusReg.Set(StatusFlag::Overflow, (result ^ m_Regs.A) & (result ^ val) & 0x80);
	m_StatusReg.Set(StatusFlag::Negative, result & 0x80);

	return AddsCycle(addrMode);
}

uint8_t CPU::AND(AddrMode addrMode)
{
	const uint8_t val = Read(ResolveAddress(addrMode));
	m_Regs.A &= val;
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);

	return AddsCycle(addrMode);
}

uint8_t CPU::ASL(AddrMode addrMode)
{
	if (addrMode == AddrMode::Accumulator)
	{
		m_StatusReg.Set(StatusFlag::Carry, m_Regs.A & 0x80);
		m_Regs.A <<= 1;
		m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
		m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
	}
	else
	{
		const uint16_t addr = ResolveAddress(addrMode);
		uint8_t val = Read(addr);
		Write(addr, val);
		m_StatusReg.Set(StatusFlag::Carry, val & 0x80);
		val <<= 1;
		m_StatusReg.Set(StatusFlag::Zero, val == 0);
		m_StatusReg.Set(StatusFlag::Negative, val & 0x80);
		Write(addr, val);
	}
	return 0;
}

uint8_t CPU::BCC(AddrMode addrMode)
{
	if (!m_StatusReg.Test(StatusFlag::Carry))
		return Branch(ResolveAddress(addrMode));
	return 0;
}

uint8_t CPU::BCS(AddrMode addrMode)
{ 
	if (m_StatusReg.Test(StatusFlag::Carry))
		return Branch(ResolveAddress(addrMode));
	return 0;
}

uint8_t CPU::BEQ(AddrMode addrMode)
{
	if (m_StatusReg.Test(StatusFlag::Zero))
		return Branch(ResolveAddress(addrMode));
	return 0;
}

uint8_t CPU::BIT(AddrMode addrMode)
{
	const uint8_t val = Read(ResolveAddress(addrMode));
	const uint8_t result = m_Regs.A & val;
	m_StatusReg.Set(StatusFlag::Zero, result == 0);
	m_StatusReg.Set(StatusFlag::Overflow, val & 0x40);
	m_StatusReg.Set(StatusFlag::Negative, val & 0x80);
	return 0;
}

uint8_t CPU::BMI(AddrMode addrMode)
{
	if (m_StatusReg.Test(StatusFlag::Negative))
		return Branch(ResolveAddress(addrMode));
	return 0;
}

uint8_t CPU::BNE(AddrMode addrMode)
{
	if (!m_StatusReg.Test(StatusFlag::Zero))
		return Branch(ResolveAddress(addrMode));
	return 0;
}

uint8_t CPU::BPL(AddrMode addrMode)
{
	if (!m_StatusReg.Test(StatusFlag::Negative))
		return Branch(ResolveAddress(addrMode));
	return 0;
}

uint8_t CPU::BRK(AddrMode addrMode)
{
	PushStackWord(m_Regs.PC + 2);
	PushStack((m_StatusReg | StatusFlag::Break).GetRaw());
	
	m_StatusReg.Set(StatusFlag::InterruptDisable);
	m_Regs.PC = ReadWord(0xFFFE);
	m_PCManuallySet = true;
	return 0;
}

uint8_t CPU::BVC(AddrMode addrMode)
{
	if (!m_StatusReg.Test(StatusFlag::Overflow))
		Branch(ResolveAddress(addrMode));
	return 0;
}

uint8_t CPU::BVS(AddrMode addrMode)
{
	if (m_StatusReg.Test(StatusFlag::Overflow))
		Branch(ResolveAddress(addrMode));
	return 0;
}

uint8_t CPU::CLC(AddrMode addrMode)
{
	m_StatusReg.Clear(StatusFlag::Carry);
	return 0;
}

uint8_t CPU::CLD(AddrMode addrMode)
{
	m_StatusReg.Clear(StatusFlag::Decimal);
	return 0;
}

uint8_t CPU::CLI(AddrMode addrMode)
{
	m_InterruptDisableDelay = INTERRUPT_DISABLE_DELAY_OFF;
	return 0;
}

uint8_t CPU::CLV(AddrMode addrMode)
{
	m_StatusReg.Clear(StatusFlag::Overflow);
	return 0;
}

uint8_t CPU::CMP(AddrMode addrMode)
{
	const uint8_t val = Read(ResolveAddress(addrMode));
	m_StatusReg.Set(StatusFlag::Carry, m_Regs.A >= val);
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == val);
	m_StatusReg.Set(StatusFlag::Negative, (m_Regs.A - val) & 0x80);
	return AddsCycle(addrMode);
}

uint8_t CPU::CPX(AddrMode addrMode)
{
	const uint8_t val = Read(ResolveAddress(addrMode));
	m_StatusReg.Set(StatusFlag::Carry, m_Regs.X >= val);
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.X == val);
	m_StatusReg.Set(StatusFlag::Negative, (m_Regs.X - val) & 0x80);
	return 0;
}

uint8_t CPU::CPY(AddrMode addrMode)
{
	const uint8_t val = Read(ResolveAddress(addrMode));
	m_StatusReg.Set(StatusFlag::Carry, m_Regs.Y >= val);
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.Y == val);
	m_StatusReg.Set(StatusFlag::Negative, (m_Regs.Y - val) & 0x80);
	return 0;
}

uint8_t CPU::DEC(AddrMode addrMode)
{
	const uint16_t addr = ResolveAddress(addrMode);
	uint8_t val = Read(addr);
	Write(addr, val);
	val -= 1;
	m_StatusReg.Set(StatusFlag::Zero, val == 0);
	m_StatusReg.Set(StatusFlag::Negative, val & 0x80);
	Write(addr, val);
	return 0;
}

uint8_t CPU::DEX(AddrMode addrMode)
{
	m_Regs.X--;
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.X == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.X & 0x80);
	return 0;
}

uint8_t CPU::DEY(AddrMode addrMode)
{
	m_Regs.Y--;
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.Y == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.Y & 0x80);
	return 0;
}

uint8_t CPU::EOR(AddrMode addrMode)
{
	const uint8_t val = Read(ResolveAddress(addrMode));
	m_Regs.A ^= val;
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A & 0x80);
	return AddsCycle(addrMode);
}

uint8_t CPU::INC(AddrMode addrMode)
{
	const uint16_t addr = ResolveAddress(addrMode);
	uint8_t val = Read(addr);
	Write(addr, val);
	val++;
	m_StatusReg.Set(StatusFlag::Zero, val == 0);
	m_StatusReg.Set(StatusFlag::Negative, val & 0x80);
	Write(addr, val);
	return 0;
}

uint8_t CPU::INX(AddrMode addrMode)
{
	m_Regs.X++;
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.X == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.X & 0x80);
	return 0;
}

uint8_t CPU::INY(AddrMode addrMode)
{
	m_Regs.Y++;
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.Y == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.Y & 0x80);
	return 0;
}

uint8_t CPU::JMP(AddrMode addrMode)
{
	m_Regs.PC = ResolveAddress(addrMode);
	m_PCManuallySet = true;
	return 0;
}

uint8_t CPU::JSR(AddrMode addrMode)
{
	PushStackWord(m_Regs.PC + 2);
	m_Regs.PC = ResolveAddress(addrMode);
	m_PCManuallySet = true;
	return 0;
}

uint8_t CPU::LDA(AddrMode addrMode)
{
	m_Regs.A = Read(ResolveAddress(addrMode));
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
	return 0;
}

uint8_t CPU::LDX(AddrMode addrMode)
{
	m_Regs.X = Read(ResolveAddress(addrMode));
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.X == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.X & 0x80);
	return 0;
}

uint8_t CPU::LDY(AddrMode addrMode)
{
	m_Regs.Y = Read(ResolveAddress(addrMode));
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.Y == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.Y & 0x80);
	return 0;
}

uint8_t CPU::LSR(AddrMode addrMode)
{
	if (addrMode == AddrMode::Accumulator)
	{
		m_StatusReg.Set(StatusFlag::Carry, m_Regs.A & 0x1);
		m_Regs.A >>= 1;
		m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
		m_StatusReg.Clear(StatusFlag::Negative);
	}
	else
	{
		const uint16_t addr = ResolveAddress(addrMode);
		uint8_t val = Read(addr);
		Write(addr, val);
		m_StatusReg.Set(StatusFlag::Carry, val & 0x1);
		val >>= 1;
		m_StatusReg.Set(StatusFlag::Zero, val == 0);
		m_StatusReg.Clear(StatusFlag::Negative);
		Write(addr, val);
	}
	return 0;
}

uint8_t CPU::NOP(AddrMode addrMode)
{
	return 0;
}

uint8_t CPU::ORA(AddrMode addrMode)
{
	const uint8_t val = Read(ResolveAddress(addrMode));
	m_Regs.A |= val;
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
	return AddsCycle(addrMode);
}

uint8_t CPU::PHA(AddrMode addrMode)
{
	PushStack(m_Regs.A);
	return 0;
}

uint8_t CPU::PHP(AddrMode addrMode)
{
	PushStack((m_StatusReg | StatusFlag::Break).GetRaw());
	return 0;
}

uint8_t CPU::PLA(AddrMode addrMode)
{
	m_Regs.A = PopStack();
	return 0;
}

uint8_t CPU::PLP(AddrMode addrMode)
{
	const uint8_t val = PopStack();
	const uint8_t mask = static_cast<uint8_t>(StatusFlag::InterruptDisable);
	const bool interruptDisable = val & mask;
	const uint8_t src = val & ~mask;
	m_StatusReg = StatusRegister{ static_cast<uint8_t>((m_StatusReg.GetRaw() & mask) | src) };
	m_InterruptDisableDelay = interruptDisable ? INTERRUPT_DISABLE_DELAY_ON : INTERRUPT_DISABLE_DELAY_OFF;
	return 0;
}

uint8_t CPU::ROL(AddrMode addrMode)
{
	const uint16_t addr = ResolveAddress(addrMode);
	const uint8_t val = Read(addr);
	Write(addr, val);
	const uint8_t res = (val << 1) | m_StatusReg.Test(StatusFlag::Carry);
	m_StatusReg.Set(StatusFlag::Carry, val & 0x80);
	m_StatusReg.Set(StatusFlag::Zero, res == 0);
	m_StatusReg.Set(StatusFlag::Negative, res & 0x80);
	Write(addr, res);
	return 0;
}

uint8_t CPU::ROR(AddrMode addrMode)
{
	const uint16_t addr = ResolveAddress(addrMode);
	const uint8_t val = Read(addr);
	Write(addr, val);
	const uint8_t res = (val >> 1) | (m_StatusReg.Test(StatusFlag::Carry) << 7);
	m_StatusReg.Set(StatusFlag::Carry, val & 0x1);
	m_StatusReg.Set(StatusFlag::Zero, res == 0);
	m_StatusReg.Set(StatusFlag::Negative, res & 0x80);
	Write(addr, res);
	return 0;
}

uint8_t CPU::RTI(AddrMode addrMode)
{
	m_StatusReg = StatusRegister{ PopStack() };
	m_Regs.PC = PopStackWord();
	m_PCManuallySet = true;
	return 0;
}

uint8_t CPU::RTS(AddrMode addrMode)
{
	m_Regs.PC = PopStackWord() + 1;
	m_PCManuallySet = true;
	return 0;
}

uint8_t CPU::SBC(AddrMode addrMode)
{
	const uint8_t carry = m_StatusReg.Test(StatusFlag::Carry);
	const uint8_t val = Read(ResolveAddress(addrMode));
	const uint16_t res = m_Regs.A + ~val + carry;
	m_StatusReg.Set(StatusFlag::Carry, res <= 0xFF);
	m_StatusReg.Set(StatusFlag::Zero, (res & 0xFF) == 0);
	m_StatusReg.Set(StatusFlag::Overflow, (res ^ m_Regs.A) & (res ^ ~val) & 0x80);
	m_StatusReg.Set(StatusFlag::Negative, res & 0x80);
	m_Regs.A = res & 0xFF;
	return 0;
}

uint8_t CPU::SEC(AddrMode addrMode)
{
	m_StatusReg.Set(StatusFlag::Carry);
	return 0;
}

uint8_t CPU::SED(AddrMode addrMode)
{
	m_StatusReg.Set(StatusFlag::Decimal);
	return 0;
}

uint8_t CPU::SEI(AddrMode addrMode)
{
	m_InterruptDisableDelay = INTERRUPT_DISABLE_DELAY_ON;
	return 0;
}

uint8_t CPU::STA(AddrMode addrMode)
{
	Write(ResolveAddress(addrMode), m_Regs.A);
	return 0;
}

uint8_t CPU::STX(AddrMode addrMode)
{
	Write(ResolveAddress(addrMode), m_Regs.X);
	return 0;
}

uint8_t CPU::STY(AddrMode addrMode)
{
	Write(ResolveAddress(addrMode), m_Regs.Y);
	return 0;
}

uint8_t CPU::TAX(AddrMode addrMode)
{
	m_Regs.X = m_Regs.A;
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.X == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.X & 0x80);
	return 0;
}

uint8_t CPU::TAY(AddrMode addrMode)
{
	m_Regs.Y = m_Regs.A;
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.Y == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.Y & 0x80);
	return 0;
}

uint8_t CPU::TSX(AddrMode addrMode)
{
	m_Regs.X = m_Regs.S;
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.X == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.X & 0x80);
	return 0;
}

uint8_t CPU::TXA(AddrMode addrMode)
{
	m_Regs.A = m_Regs.X;
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
	return 0;
}

uint8_t CPU::TXS(AddrMode addrMode)
{
	m_Regs.S = m_Regs.X;
	return 0;
}

uint8_t CPU::TYA(AddrMode addrMode)
{
	m_Regs.A = m_Regs.Y;
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
	return 0;
}