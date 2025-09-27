#include "CPU.h"

#include "Bus.h"
#include "Common.h"
#include "DebugUtils.h"
#include <stdio.h>
#include <string.h>

static constexpr uint8_t INTERRUPT_DISABLE_DELAY_NONE = 0x0;
static constexpr uint8_t INTERRUPT_DISABLE_DELAY_OFF = 0x1;
static constexpr uint8_t INTERRUPT_DISABLE_DELAY_ON = 0x3;

static constexpr uint16_t STACK_BEGIN = 0x0100;

const std::array<Instruction, 256> CPU::s_OpcodeLookup = [] {
	std::array<Instruction, 256> lookup{};

	lookup[0x69] = { InstrType::ADC, AddrMode::Immediate, 2, 2 };
	lookup[0x65] = { InstrType::ADC, AddrMode::ZeroPage, 2, 3 };
	lookup[0x75] = { InstrType::ADC, AddrMode::ZeroPageX, 2, 4 };
	lookup[0x6D] = { InstrType::ADC, AddrMode::Absolute, 3, 4 };
	lookup[0x7D] = { InstrType::ADC, AddrMode::AbsoluteX, 3, 4 };
	lookup[0x79] = { InstrType::ADC, AddrMode::AbsoluteY, 3, 4 };
	lookup[0x61] = { InstrType::ADC, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0x71] = { InstrType::ADC, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0x29] = { InstrType::AND, AddrMode::Immediate, 2, 2 };
	lookup[0x25] = { InstrType::AND, AddrMode::ZeroPage, 2, 3 };
	lookup[0x35] = { InstrType::AND, AddrMode::ZeroPageX, 2, 4 };
	lookup[0x2D] = { InstrType::AND, AddrMode::Absolute, 3, 4 };
	lookup[0x3D] = { InstrType::AND, AddrMode::AbsoluteX, 3, 4 };
	lookup[0x39] = { InstrType::AND, AddrMode::AbsoluteY, 3, 4 };
	lookup[0x21] = { InstrType::AND, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0x31] = { InstrType::AND, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0x0A] = { InstrType::ASL, AddrMode::Accumulator, 1, 2 };
	lookup[0x06] = { InstrType::ASL, AddrMode::ZeroPage, 2, 5 };
	lookup[0x16] = { InstrType::ASL, AddrMode::ZeroPageX, 2, 6 };
	lookup[0x0E] = { InstrType::ASL, AddrMode::Absolute, 3, 6 };
	lookup[0x1E] = { InstrType::ASL, AddrMode::AbsoluteX, 3, 7 };

	lookup[0x90] = { InstrType::BCC, AddrMode::Relative, 2, 2 };

	lookup[0xB0] = { InstrType::BCS, AddrMode::Relative, 2, 2 };

	lookup[0xF0] = { InstrType::BEQ, AddrMode::Relative, 2, 2 };

	lookup[0x24] = { InstrType::BIT, AddrMode::ZeroPage, 2, 3 };
	lookup[0x2C] = { InstrType::BIT, AddrMode::Absolute, 3, 4 };

	lookup[0x30] = { InstrType::BMI, AddrMode::Relative, 2, 2 };

	lookup[0xD0] = { InstrType::BNE, AddrMode::Relative, 2, 2 };

	lookup[0x10] = { InstrType::BPL, AddrMode::Relative, 2, 2 };

	lookup[0x00] = { InstrType::BRK, AddrMode::Implicit, 1, 7 };

	lookup[0x50] = { InstrType::BVC, AddrMode::Relative, 2, 2 };

	lookup[0x70] = { InstrType::BVS, AddrMode::Relative, 2, 2 };

	lookup[0x18] = { InstrType::CLC, AddrMode::Implicit, 1, 2 };

	lookup[0xD8] = { InstrType::CLD, AddrMode::Implicit, 1, 2 };

	lookup[0x58] = { InstrType::CLI, AddrMode::Implicit, 1, 2 };

	lookup[0xB8] = { InstrType::CLV, AddrMode::Implicit, 1, 2 };

	lookup[0xC9] = { InstrType::CMP, AddrMode::Immediate, 2, 2 };
	lookup[0xC5] = { InstrType::CMP, AddrMode::ZeroPage, 2, 3 };
	lookup[0xD5] = { InstrType::CMP, AddrMode::ZeroPageX, 2, 4 };
	lookup[0xCD] = { InstrType::CMP, AddrMode::Absolute, 3, 4 };
	lookup[0xDD] = { InstrType::CMP, AddrMode::AbsoluteX, 3, 4 };
	lookup[0xD9] = { InstrType::CMP, AddrMode::AbsoluteY, 3, 4 };
	lookup[0xC1] = { InstrType::CMP, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0xD1] = { InstrType::CMP, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0xE0] = { InstrType::CPX, AddrMode::Immediate, 2, 2 };
	lookup[0xE4] = { InstrType::CPX, AddrMode::ZeroPage, 2, 3 };
	lookup[0xEC] = { InstrType::CPX, AddrMode::Absolute, 3, 4 };

	lookup[0xC0] = { InstrType::CPY, AddrMode::Immediate, 2, 2 };
	lookup[0xC4] = { InstrType::CPY, AddrMode::ZeroPage, 2, 3 };
	lookup[0xCC] = { InstrType::CPY, AddrMode::Absolute, 3, 4 };

	lookup[0xC6] = { InstrType::DEC, AddrMode::ZeroPage, 2, 5 };
	lookup[0xD6] = { InstrType::DEC, AddrMode::ZeroPageX, 2, 6 };
	lookup[0xCE] = { InstrType::DEC, AddrMode::Absolute, 3, 6 };
	lookup[0xDE] = { InstrType::DEC, AddrMode::AbsoluteX, 3, 7 };

	lookup[0xCA] = { InstrType::DEX, AddrMode::Implicit, 1, 2 };

	lookup[0x88] = { InstrType::DEY, AddrMode::Implicit, 1, 2 };

	lookup[0x49] = { InstrType::EOR, AddrMode::Immediate, 2, 2 };
	lookup[0x45] = { InstrType::EOR, AddrMode::ZeroPage, 2, 3 };
	lookup[0x55] = { InstrType::EOR, AddrMode::ZeroPageX, 2, 4 };
	lookup[0x4D] = { InstrType::EOR, AddrMode::Absolute, 3, 4 };
	lookup[0x5D] = { InstrType::EOR, AddrMode::AbsoluteX, 3, 4 };
	lookup[0x59] = { InstrType::EOR, AddrMode::AbsoluteY, 3, 4 };
	lookup[0x41] = { InstrType::EOR, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0x51] = { InstrType::EOR, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0xE6] = { InstrType::INC, AddrMode::ZeroPage, 2, 5 };
	lookup[0xF6] = { InstrType::INC, AddrMode::ZeroPageX, 2, 6 };
	lookup[0xEE] = { InstrType::INC, AddrMode::Absolute, 3, 6 };
	lookup[0xFE] = { InstrType::INC, AddrMode::AbsoluteX, 3, 7 };

	lookup[0xE8] = { InstrType::INX, AddrMode::Implicit, 1, 2 };

	lookup[0xC8] = { InstrType::INY, AddrMode::Implicit, 1, 2 };

	lookup[0x4C] = { InstrType::JMP, AddrMode::Absolute, 3, 3 };
	lookup[0x6C] = { InstrType::JMP, AddrMode::Indirect, 3, 5 };

	lookup[0x20] = { InstrType::JSR, AddrMode::Absolute, 3, 6 };

	lookup[0xA9] = { InstrType::LDA, AddrMode::Immediate, 2, 2 };
	lookup[0xA5] = { InstrType::LDA, AddrMode::ZeroPage, 2, 3 };
	lookup[0xB5] = { InstrType::LDA, AddrMode::ZeroPageX, 2, 4 };
	lookup[0xAD] = { InstrType::LDA, AddrMode::Absolute, 3, 4 };
	lookup[0xBD] = { InstrType::LDA, AddrMode::AbsoluteX, 3, 4 };
	lookup[0xB9] = { InstrType::LDA, AddrMode::AbsoluteY, 3, 4 };
	lookup[0xA1] = { InstrType::LDA, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0xB1] = { InstrType::LDA, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0xA2] = { InstrType::LDX, AddrMode::Immediate, 2, 2 };
	lookup[0xA6] = { InstrType::LDX, AddrMode::ZeroPage, 2, 3 };
	lookup[0xB6] = { InstrType::LDX, AddrMode::ZeroPageY, 2, 4 };
	lookup[0xAE] = { InstrType::LDX, AddrMode::Absolute, 3, 4 };
	lookup[0xBE] = { InstrType::LDX, AddrMode::AbsoluteY, 3, 4 };

	lookup[0xA0] = { InstrType::LDY, AddrMode::Immediate, 2, 2 };
	lookup[0xA4] = { InstrType::LDY, AddrMode::ZeroPage, 2, 3 };
	lookup[0xB4] = { InstrType::LDY, AddrMode::ZeroPageX, 2, 4 };
	lookup[0xAC] = { InstrType::LDY, AddrMode::Absolute, 3, 4 };
	lookup[0xBC] = { InstrType::LDY, AddrMode::AbsoluteX, 3, 4 };

	lookup[0x4A] = { InstrType::LSR, AddrMode::Accumulator, 1, 2 };
	lookup[0x46] = { InstrType::LSR, AddrMode::ZeroPage, 2, 5 };
	lookup[0x56] = { InstrType::LSR, AddrMode::ZeroPageX, 2, 6 };
	lookup[0x4E] = { InstrType::LSR, AddrMode::Absolute, 3, 6 };
	lookup[0x5E] = { InstrType::LSR, AddrMode::AbsoluteX, 3, 7 };

	lookup[0xEA] = { InstrType::NOP, AddrMode::Implicit, 1, 2 };

	lookup[0x09] = { InstrType::ORA, AddrMode::Immediate, 2, 2 };
	lookup[0x05] = { InstrType::ORA, AddrMode::ZeroPage, 2, 3 };
	lookup[0x15] = { InstrType::ORA, AddrMode::ZeroPageX, 2, 4 };
	lookup[0x0D] = { InstrType::ORA, AddrMode::Absolute, 3, 4 };
	lookup[0x1D] = { InstrType::ORA, AddrMode::AbsoluteX, 3, 4 };
	lookup[0x19] = { InstrType::ORA, AddrMode::AbsoluteY, 3, 4 };
	lookup[0x01] = { InstrType::ORA, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0x11] = { InstrType::ORA, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0x48] = { InstrType::PHA, AddrMode::Implicit, 1, 3 };

	lookup[0x08] = { InstrType::PHP, AddrMode::Implicit, 1, 3 };

	lookup[0x68] = { InstrType::PLA, AddrMode::Implicit, 1, 4 };

	lookup[0x28] = { InstrType::PLP, AddrMode::Implicit, 1, 4 };

	lookup[0x2A] = { InstrType::ROL, AddrMode::Accumulator, 1, 2 };
	lookup[0x26] = { InstrType::ROL, AddrMode::ZeroPage, 2, 5 };
	lookup[0x36] = { InstrType::ROL, AddrMode::ZeroPageX, 2, 6 };
	lookup[0x2E] = { InstrType::ROL, AddrMode::Absolute, 3, 6 };
	lookup[0x3E] = { InstrType::ROL, AddrMode::AbsoluteX, 3, 7 };

	lookup[0x6A] = { InstrType::ROR, AddrMode::Accumulator, 1, 2 };
	lookup[0x66] = { InstrType::ROR, AddrMode::ZeroPage, 2, 5 };
	lookup[0x76] = { InstrType::ROR, AddrMode::ZeroPageX, 2, 6 };
	lookup[0x6E] = { InstrType::ROR, AddrMode::Absolute, 3, 6 };
	lookup[0x7E] = { InstrType::ROR, AddrMode::AbsoluteX, 3, 7 };

	lookup[0x40] = { InstrType::RTI, AddrMode::Implicit, 1, 6 };

	lookup[0x60] = { InstrType::RTS, AddrMode::Implicit, 1, 6 };

	lookup[0xE9] = { InstrType::SBC, AddrMode::Immediate, 2, 2 };
	lookup[0xE5] = { InstrType::SBC, AddrMode::ZeroPage, 2, 3 };
	lookup[0xF5] = { InstrType::SBC, AddrMode::ZeroPageX, 2, 4 };
	lookup[0xED] = { InstrType::SBC, AddrMode::Absolute, 3, 4 };
	lookup[0xFD] = { InstrType::SBC, AddrMode::AbsoluteX, 3, 4 };
	lookup[0xF9] = { InstrType::SBC, AddrMode::AbsoluteY, 3, 4 };
	lookup[0xE1] = { InstrType::SBC, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0xF1] = { InstrType::SBC, AddrMode::IndirectIndexed, 2, 5 };

	lookup[0x38] = { InstrType::SEC, AddrMode::Implicit, 1, 2 };

	lookup[0xF8] = { InstrType::SED, AddrMode::Implicit, 1, 2 };

	lookup[0x78] = { InstrType::SEI, AddrMode::Implicit, 1, 2 };

	lookup[0x85] = { InstrType::STA, AddrMode::ZeroPage, 2, 3 };
	lookup[0x95] = { InstrType::STA, AddrMode::ZeroPageX, 2, 4 };
	lookup[0x8D] = { InstrType::STA, AddrMode::Absolute, 3, 4 };
	lookup[0x9D] = { InstrType::STA, AddrMode::AbsoluteX, 3, 5 };
	lookup[0x99] = { InstrType::STA, AddrMode::AbsoluteY, 3, 5 };
	lookup[0x81] = { InstrType::STA, AddrMode::IndexedIndirect, 2, 6 };
	lookup[0x91] = { InstrType::STA, AddrMode::IndirectIndexed, 2, 6 };

	lookup[0x86] = { InstrType::STX, AddrMode::ZeroPage, 2, 3 };
	lookup[0x96] = { InstrType::STX, AddrMode::ZeroPageY, 2, 4 };
	lookup[0x8E] = { InstrType::STX, AddrMode::Absolute, 3, 4 };

	lookup[0x84] = { InstrType::STY, AddrMode::ZeroPage, 2, 3 };
	lookup[0x94] = { InstrType::STY, AddrMode::ZeroPageX, 2, 4 };
	lookup[0x8C] = { InstrType::STY, AddrMode::Absolute, 3, 4 };

	lookup[0xAA] = { InstrType::TAX, AddrMode::Implicit, 1, 2 };

	lookup[0xA8] = { InstrType::TAY, AddrMode::Implicit, 1, 2 };

	lookup[0xBA] = { InstrType::TSX, AddrMode::Implicit, 1, 2 };

	lookup[0x8A] = { InstrType::TXA, AddrMode::Implicit, 1, 2 };

	lookup[0x9A] = { InstrType::TXS, AddrMode::Implicit, 1, 2 };

	lookup[0x98] = { InstrType::TYA, AddrMode::Implicit, 1, 2 };

	return lookup;
}();

void CPU::Init(Bus* bus)
{
	m_Bus = bus;
	m_Regs.A = 0;
	m_Regs.X = 0;
	m_Regs.Y = 0;
	m_Regs.PC = 0;
	m_Regs.S = 0xFD;
	m_StatusReg = StatusFlag::InterruptDisable | StatusFlag::Unused;
}

void CPU::Reset()
{
	m_Regs.PC = 0xC000;
	m_Regs.S = 0xFD;
	m_StatusReg.Set(StatusFlag::InterruptDisable);
	m_InstructionRemainingCycles = 7;
}

void CPU::PerformCycle()
{
	if (m_InstructionRemainingCycles > 0)
	{
		m_InstructionRemainingCycles--;
		m_TotalCycles++;
		return;
	}

	//if (m_InterruptDisableDelay != INTERRUPT_DISABLE_DELAY_NONE)
	//{
	//	m_StatusReg.Set(StatusFlag::InterruptDisable, m_InterruptDisableDelay == INTERRUPT_DISABLE_DELAY_ON);
	//}

	const uint8_t opCode = Read(m_Regs.PC);
	
	m_CurrentInstruction = s_OpcodeLookup[opCode];
	PrintState();
	ExecuteInstruction();

	if (!m_PCManuallySet)
		m_Regs.PC += m_CurrentInstruction.byteCount;

	m_InterruptDisableDelay = INTERRUPT_DISABLE_DELAY_NONE;
	m_PCManuallySet = false;
}

void CPU::PrintState() const
{
	char buf[128];

	const int instrBytes = m_CurrentInstruction.byteCount;

	char addrBuf[32];

	uint16_t operand = 0;
	if (m_CurrentInstruction.byteCount == 2)
		operand = Read(m_Regs.PC + 1);
	if (m_CurrentInstruction.byteCount == 3)
		operand = ReadWord(m_Regs.PC + 1);

	const uint16_t addr = ResolveAddress(m_CurrentInstruction.addrMode);

	DebugUtils::AddrModeToStr(
		m_CurrentInstruction.addrMode,
		operand,
		addr,
		Read(addr),
		addrBuf,
		sizeof(addrBuf));

	const int n = std::snprintf(buf, sizeof(buf),
		"%04X  %02X %02X %02X  %s %-32s A:%02X X:%02X Y:%02X P:%02X S:%02X CYC:%llu\n",
		m_Regs.PC,
		Read(m_Regs.PC),
		instrBytes > 1 ? Read(m_Regs.PC + 1) : 0,
		instrBytes > 2 ? Read(m_Regs.PC + 2) : 0,
		DebugUtils::InstrTypeToStr(m_CurrentInstruction.instrType),
		addrBuf,
		m_Regs.A, m_Regs.X, m_Regs.Y, m_StatusReg.GetRaw(), m_Regs.S,
		m_TotalCycles
	);
	
	if (instrBytes < 2)
	{
		buf[9] = ' ';
		buf[10] = ' ';
	}
	if (instrBytes < 3)
	{
		buf[12] = ' ';
		buf[13] = ' ';
	}

	std::fwrite(buf, 1, n, stdout);
}

void CPU::ExecuteInstruction()
{
	const InstructionFunc f = ResolveInstructionFunction(m_CurrentInstruction.instrType);

	const uint8_t addCycles = (this->*f)(m_CurrentInstruction.addrMode);
	m_InstructionRemainingCycles = addCycles + m_CurrentInstruction.cycleCount;
}

uint8_t CPU::Read(uint16_t addr) const
{
	return m_Bus->Read(addr);
}

uint16_t CPU::ReadWord(uint16_t addr) const
{
	const uint8_t low = m_Bus->Read(addr);
	const uint8_t high = m_Bus->Read(addr + 1);
	return low | (high << 8);
}

void CPU::Write(uint16_t addr, uint8_t val)
{
	return m_Bus->Write(addr, val);
}

void CPU::WriteWord(uint16_t addr, uint16_t val)
{
	const uint8_t low = val & 0xFF;
	const uint8_t high = (val >> 8) & 0xFF;
	m_Bus->Write(addr, low);
	m_Bus->Write(addr + 1, high);
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

uint8_t CPU::Branch(uint16_t addr)
{
	const uint16_t updated = addr;
	uint8_t addCycles = 1;
	if ((updated & 0xFF00) != ((m_Regs.PC + m_CurrentInstruction.byteCount) & 0xFF00))
		addCycles++;
	m_Regs.PC = updated;
	m_PCManuallySet = true;
	return addCycles;
}

uint16_t CPU::ResolveAddress(AddrMode addrMode) const
{
	switch (addrMode)
	{
	case AddrMode::None: return 0;
	case AddrMode::Implicit: return ResolveImplicit();
	case AddrMode::Accumulator: return ResolveAccumulator();
	case AddrMode::Immediate: return ResolveImmediate();
	case AddrMode::ZeroPage: return ResolveZeroPage();
	case AddrMode::ZeroPageX: return ResolveZeroPageX();
	case AddrMode::ZeroPageY: return ResolveZeroPageY();
	case AddrMode::Relative: return ResolveRelative();
	case AddrMode::Absolute: return ResolveAbsolute();
	case AddrMode::AbsoluteX: return ResolveAbsoluteX();
	case AddrMode::AbsoluteY: return ResolveAbsoluteY();
	case AddrMode::Indirect: return ResolveIndirect();
	case AddrMode::IndexedIndirect: return ResolveIndexedIndirect();
	case AddrMode::IndirectIndexed: return ResolveIndirectIndexed();
	default: return 0;
	}
}

InstructionFunc CPU::ResolveInstructionFunction(InstrType instrType) const
{
	switch (instrType)
	{
	case InstrType::None: return nullptr;
	case InstrType::ADC:  return &CPU::ADC;
	case InstrType::AND:  return &CPU::AND;
	case InstrType::ASL:  return &CPU::ASL;
	case InstrType::BCC:  return &CPU::BCC;
	case InstrType::BCS:  return &CPU::BCS;
	case InstrType::BEQ:  return &CPU::BEQ;
	case InstrType::BIT:  return &CPU::BIT;
	case InstrType::BMI:  return &CPU::BMI;
	case InstrType::BNE:  return &CPU::BNE;
	case InstrType::BPL:  return &CPU::BPL;
	case InstrType::BRK:  return &CPU::BRK;
	case InstrType::BVC:  return &CPU::BVC;
	case InstrType::BVS:  return &CPU::BVS;
	case InstrType::CLC:  return &CPU::CLC;
	case InstrType::CLD:  return &CPU::CLD;
	case InstrType::CLI:  return &CPU::CLI;
	case InstrType::CLV:  return &CPU::CLV;
	case InstrType::CMP:  return &CPU::CMP;
	case InstrType::CPX:  return &CPU::CPX;
	case InstrType::CPY:  return &CPU::CPY;
	case InstrType::DEC:  return &CPU::DEC;
	case InstrType::DEX:  return &CPU::DEX;
	case InstrType::DEY:  return &CPU::DEY;
	case InstrType::EOR:  return &CPU::EOR;
	case InstrType::INC:  return &CPU::INC;
	case InstrType::INX:  return &CPU::INX;
	case InstrType::INY:  return &CPU::INY;
	case InstrType::JMP:  return &CPU::JMP;
	case InstrType::JSR:  return &CPU::JSR;
	case InstrType::LDA:  return &CPU::LDA;
	case InstrType::LDX:  return &CPU::LDX;
	case InstrType::LDY:  return &CPU::LDY;
	case InstrType::LSR:  return &CPU::LSR;
	case InstrType::NOP:  return &CPU::NOP;
	case InstrType::ORA:  return &CPU::ORA;
	case InstrType::PHA:  return &CPU::PHA;
	case InstrType::PHP:  return &CPU::PHP;
	case InstrType::PLA:  return &CPU::PLA;
	case InstrType::PLP:  return &CPU::PLP;
	case InstrType::ROL:  return &CPU::ROL;
	case InstrType::ROR:  return &CPU::ROR;
	case InstrType::RTI:  return &CPU::RTI;
	case InstrType::RTS:  return &CPU::RTS;
	case InstrType::SBC:  return &CPU::SBC;
	case InstrType::SEC:  return &CPU::SEC;
	case InstrType::SED:  return &CPU::SED;
	case InstrType::SEI:  return &CPU::SEI;
	case InstrType::STA:  return &CPU::STA;
	case InstrType::STX:  return &CPU::STX;
	case InstrType::STY:  return &CPU::STY;
	case InstrType::TAX:  return &CPU::TAX;
	case InstrType::TAY:  return &CPU::TAY;
	case InstrType::TSX:  return &CPU::TSX;
	case InstrType::TXA:  return &CPU::TXA;
	case InstrType::TXS:  return &CPU::TXS;
	case InstrType::TYA:  return &CPU::TYA;
	default: return nullptr;
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
	uint16_t addr = Read(m_Regs.PC + 1);
	if (addr & 0x80)
	{
		addr |= 0xFF00;
	}
	return addr + m_Regs.PC + 2;
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
	m_StatusReg.Set(StatusFlag::Zero, (result & 0xFF) == 0);
	m_StatusReg.Set(StatusFlag::Overflow, (result ^ m_Regs.A) & (result ^ val) & 0x80);
	m_StatusReg.Set(StatusFlag::Negative, result & 0x80);

	m_Regs.A = result & 0xFF;

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
		return Branch(ResolveAddress(addrMode));
	return 0;
}

uint8_t CPU::BVS(AddrMode addrMode)
{
	if (m_StatusReg.Test(StatusFlag::Overflow))
		return Branch(ResolveAddress(addrMode));
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
	m_StatusReg.Set(StatusFlag::InterruptDisable, false);
	//m_InterruptDisableDelay = INTERRUPT_DISABLE_DELAY_OFF;
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
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
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
	return AddsCycle(addrMode);
}

uint8_t CPU::LDX(AddrMode addrMode)
{
	m_Regs.X = Read(ResolveAddress(addrMode));
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.X == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.X & 0x80);
	return AddsCycle(addrMode);
}

uint8_t CPU::LDY(AddrMode addrMode)
{
	m_Regs.Y = Read(ResolveAddress(addrMode));
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.Y == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.Y & 0x80);
	return AddsCycle(addrMode);
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
	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
	return 0;
}

uint8_t CPU::PLP(AddrMode addrMode)
{
	const StatusRegister flags{ PopStack() };
	//const bool interruptDisable = flags & StatusFlag::InterruptDisable;
	m_StatusReg
		.Set(StatusFlag::Carry, flags & StatusFlag::Carry)
		.Set(StatusFlag::Zero, flags & StatusFlag::Zero)
		.Set(StatusFlag::InterruptDisable, flags & StatusFlag::InterruptDisable)
		.Set(StatusFlag::Decimal, flags & StatusFlag::Decimal)
		.Set(StatusFlag::Overflow, flags & StatusFlag::Overflow)
		.Set(StatusFlag::Negative, flags & StatusFlag::Negative);
	//m_InterruptDisableDelay = interruptDisable ? INTERRUPT_DISABLE_DELAY_ON : INTERRUPT_DISABLE_DELAY_OFF;
	return 0;
}

uint8_t CPU::ROL(AddrMode addrMode)
{
	const uint16_t addr = ResolveAddress(addrMode);
	const uint8_t val = (addrMode == AddrMode::Accumulator) ? m_Regs.A : Read(addr);
	const uint8_t res = (val << 1) | (m_StatusReg.Test(StatusFlag::Carry) ? 1 : 0);
	m_StatusReg.Set(StatusFlag::Carry, val & 0x80);
	m_StatusReg.Set(StatusFlag::Zero, res == 0);
	m_StatusReg.Set(StatusFlag::Negative, res & 0x80);

	if (addrMode == AddrMode::Accumulator)
	{
		m_Regs.A = res;
	}
	else
	{
		Write(addr, val);
		Write(addr, res);
	}
	return 0;
}

uint8_t CPU::ROR(AddrMode addrMode)
{
	const uint16_t addr = ResolveAddress(addrMode);
	const uint8_t val = (addrMode == AddrMode::Accumulator) ? m_Regs.A : Read(addr);
	const uint8_t res = (val >> 1) | (m_StatusReg.Test(StatusFlag::Carry) << 7);
	m_StatusReg.Set(StatusFlag::Carry, val & 0x1);
	m_StatusReg.Set(StatusFlag::Zero, res == 0);
	m_StatusReg.Set(StatusFlag::Negative, res & 0x80);

	if (addrMode == AddrMode::Accumulator)
	{
		m_Regs.A = res;
	}
	else
	{
		Write(addr, val);
		Write(addr, res);
	}

	return 0;
}

uint8_t CPU::RTI(AddrMode addrMode)
{
	const StatusRegister flags{ PopStack() };
	m_StatusReg
		.Set(StatusFlag::Carry, flags & StatusFlag::Carry)
		.Set(StatusFlag::Zero, flags & StatusFlag::Zero)
		.Set(StatusFlag::InterruptDisable, flags & StatusFlag::InterruptDisable)
		.Set(StatusFlag::Decimal, flags & StatusFlag::Decimal)
		.Set(StatusFlag::Overflow, flags & StatusFlag::Overflow)
		.Set(StatusFlag::Negative, flags & StatusFlag::Negative);

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
	const uint8_t val = ~Read(ResolveAddress(addrMode));
	const uint16_t result = m_Regs.A + val + m_StatusReg.Test(StatusFlag::Carry);

	m_StatusReg.Set(StatusFlag::Carry, result > 0xFF);
	m_StatusReg.Set(StatusFlag::Zero, (result & 0xFF) == 0);
	m_StatusReg.Set(StatusFlag::Overflow, (result ^ m_Regs.A) & (result ^ val) & 0x80);
	m_StatusReg.Set(StatusFlag::Negative, result & 0x80);

	m_Regs.A = result & 0xFF;

	return AddsCycle(addrMode);
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
	m_StatusReg.Set(StatusFlag::InterruptDisable);
	//m_InterruptDisableDelay = INTERRUPT_DISABLE_DELAY_ON;
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