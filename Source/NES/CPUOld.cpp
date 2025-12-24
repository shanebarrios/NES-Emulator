//#include "CPU.h"
//
//#include "CPUBus.h"
//#include "DebugUtils.h"
//#include <stdio.h>
//#include <string.h>
//
//static constexpr u16 STACK_BEGIN = 0x0100;
//
//static constexpr u16 NMI_VECTOR = 0xFFFA;
//static constexpr u16 RESET_VECTOR = 0xFFFC;
//static constexpr u16 IRQ_VECTOR = 0xFFFE;
//
//const Array<Instruction, 256> CPU::s_OpcodeLookup = [] {
//	Array<Instruction, 256> lookup{};
//
//	lookup[0x69] = { InstrType::ADC, AddrMode::Immediate, 2, 2 };
//	lookup[0x65] = { InstrType::ADC, AddrMode::ZeroPage, 2, 3 };
//	lookup[0x75] = { InstrType::ADC, AddrMode::ZeroPageX, 2, 4 };
//	lookup[0x6D] = { InstrType::ADC, AddrMode::Absolute, 3, 4 };
//	lookup[0x7D] = { InstrType::ADC, AddrMode::AbsoluteX, 3, 4 };
//	lookup[0x79] = { InstrType::ADC, AddrMode::AbsoluteY, 3, 4 };
//	lookup[0x61] = { InstrType::ADC, AddrMode::IndexedIndirect, 2, 6 };
//	lookup[0x71] = { InstrType::ADC, AddrMode::IndirectIndexed, 2, 5 };
//
//	lookup[0x29] = { InstrType::AND, AddrMode::Immediate, 2, 2 };
//	lookup[0x25] = { InstrType::AND, AddrMode::ZeroPage, 2, 3 };
//	lookup[0x35] = { InstrType::AND, AddrMode::ZeroPageX, 2, 4 };
//	lookup[0x2D] = { InstrType::AND, AddrMode::Absolute, 3, 4 };
//	lookup[0x3D] = { InstrType::AND, AddrMode::AbsoluteX, 3, 4 };
//	lookup[0x39] = { InstrType::AND, AddrMode::AbsoluteY, 3, 4 };
//	lookup[0x21] = { InstrType::AND, AddrMode::IndexedIndirect, 2, 6 };
//	lookup[0x31] = { InstrType::AND, AddrMode::IndirectIndexed, 2, 5 };
//
//	lookup[0x0A] = { InstrType::ASL, AddrMode::Accumulator, 1, 2 };
//	lookup[0x06] = { InstrType::ASL, AddrMode::ZeroPage, 2, 5 };
//	lookup[0x16] = { InstrType::ASL, AddrMode::ZeroPageX, 2, 6 };
//	lookup[0x0E] = { InstrType::ASL, AddrMode::Absolute, 3, 6 };
//	lookup[0x1E] = { InstrType::ASL, AddrMode::AbsoluteX, 3, 7 };
//
//	lookup[0x90] = { InstrType::BCC, AddrMode::Relative, 2, 2 };
//
//	lookup[0xB0] = { InstrType::BCS, AddrMode::Relative, 2, 2 };
//
//	lookup[0xF0] = { InstrType::BEQ, AddrMode::Relative, 2, 2 };
//
//	lookup[0x24] = { InstrType::BIT, AddrMode::ZeroPage, 2, 3 };
//	lookup[0x2C] = { InstrType::BIT, AddrMode::Absolute, 3, 4 };
//
//	lookup[0x30] = { InstrType::BMI, AddrMode::Relative, 2, 2 };
//
//	lookup[0xD0] = { InstrType::BNE, AddrMode::Relative, 2, 2 };
//
//	lookup[0x10] = { InstrType::BPL, AddrMode::Relative, 2, 2 };
//
//	lookup[0x00] = { InstrType::BRK, AddrMode::Implicit, 1, 7 };
//
//	lookup[0x50] = { InstrType::BVC, AddrMode::Relative, 2, 2 };
//
//	lookup[0x70] = { InstrType::BVS, AddrMode::Relative, 2, 2 };
//
//	lookup[0x18] = { InstrType::CLC, AddrMode::Implicit, 1, 2 };
//
//	lookup[0xD8] = { InstrType::CLD, AddrMode::Implicit, 1, 2 };
//
//	lookup[0x58] = { InstrType::CLI, AddrMode::Implicit, 1, 2 };
//
//	lookup[0xB8] = { InstrType::CLV, AddrMode::Implicit, 1, 2 };
//
//	lookup[0xC9] = { InstrType::CMP, AddrMode::Immediate, 2, 2 };
//	lookup[0xC5] = { InstrType::CMP, AddrMode::ZeroPage, 2, 3 };
//	lookup[0xD5] = { InstrType::CMP, AddrMode::ZeroPageX, 2, 4 };
//	lookup[0xCD] = { InstrType::CMP, AddrMode::Absolute, 3, 4 };
//	lookup[0xDD] = { InstrType::CMP, AddrMode::AbsoluteX, 3, 4 };
//	lookup[0xD9] = { InstrType::CMP, AddrMode::AbsoluteY, 3, 4 };
//	lookup[0xC1] = { InstrType::CMP, AddrMode::IndexedIndirect, 2, 6 };
//	lookup[0xD1] = { InstrType::CMP, AddrMode::IndirectIndexed, 2, 5 };
//
//	lookup[0xE0] = { InstrType::CPX, AddrMode::Immediate, 2, 2 };
//	lookup[0xE4] = { InstrType::CPX, AddrMode::ZeroPage, 2, 3 };
//	lookup[0xEC] = { InstrType::CPX, AddrMode::Absolute, 3, 4 };
//
//	lookup[0xC0] = { InstrType::CPY, AddrMode::Immediate, 2, 2 };
//	lookup[0xC4] = { InstrType::CPY, AddrMode::ZeroPage, 2, 3 };
//	lookup[0xCC] = { InstrType::CPY, AddrMode::Absolute, 3, 4 };
//
//	lookup[0xC6] = { InstrType::DEC, AddrMode::ZeroPage, 2, 5 };
//	lookup[0xD6] = { InstrType::DEC, AddrMode::ZeroPageX, 2, 6 };
//	lookup[0xCE] = { InstrType::DEC, AddrMode::Absolute, 3, 6 };
//	lookup[0xDE] = { InstrType::DEC, AddrMode::AbsoluteX, 3, 7 };
//
//	lookup[0xCA] = { InstrType::DEX, AddrMode::Implicit, 1, 2 };
//
//	lookup[0x88] = { InstrType::DEY, AddrMode::Implicit, 1, 2 };
//
//	lookup[0x49] = { InstrType::EOR, AddrMode::Immediate, 2, 2 };
//	lookup[0x45] = { InstrType::EOR, AddrMode::ZeroPage, 2, 3 };
//	lookup[0x55] = { InstrType::EOR, AddrMode::ZeroPageX, 2, 4 };
//	lookup[0x4D] = { InstrType::EOR, AddrMode::Absolute, 3, 4 };
//	lookup[0x5D] = { InstrType::EOR, AddrMode::AbsoluteX, 3, 4 };
//	lookup[0x59] = { InstrType::EOR, AddrMode::AbsoluteY, 3, 4 };
//	lookup[0x41] = { InstrType::EOR, AddrMode::IndexedIndirect, 2, 6 };
//	lookup[0x51] = { InstrType::EOR, AddrMode::IndirectIndexed, 2, 5 };
//
//	lookup[0xE6] = { InstrType::INC, AddrMode::ZeroPage, 2, 5 };
//	lookup[0xF6] = { InstrType::INC, AddrMode::ZeroPageX, 2, 6 };
//	lookup[0xEE] = { InstrType::INC, AddrMode::Absolute, 3, 6 };
//	lookup[0xFE] = { InstrType::INC, AddrMode::AbsoluteX, 3, 7 };
//
//	lookup[0xE8] = { InstrType::INX, AddrMode::Implicit, 1, 2 };
//
//	lookup[0xC8] = { InstrType::INY, AddrMode::Implicit, 1, 2 };
//
//	lookup[0x4C] = { InstrType::JMP, AddrMode::Absolute, 3, 3 };
//	lookup[0x6C] = { InstrType::JMP, AddrMode::Indirect, 3, 5 };
//
//	lookup[0x20] = { InstrType::JSR, AddrMode::Absolute, 3, 6 };
//
//	lookup[0xA9] = { InstrType::LDA, AddrMode::Immediate, 2, 2 };
//	lookup[0xA5] = { InstrType::LDA, AddrMode::ZeroPage, 2, 3 };
//	lookup[0xB5] = { InstrType::LDA, AddrMode::ZeroPageX, 2, 4 };
//	lookup[0xAD] = { InstrType::LDA, AddrMode::Absolute, 3, 4 };
//	lookup[0xBD] = { InstrType::LDA, AddrMode::AbsoluteX, 3, 4 };
//	lookup[0xB9] = { InstrType::LDA, AddrMode::AbsoluteY, 3, 4 };
//	lookup[0xA1] = { InstrType::LDA, AddrMode::IndexedIndirect, 2, 6 };
//	lookup[0xB1] = { InstrType::LDA, AddrMode::IndirectIndexed, 2, 5 };
//
//	lookup[0xA2] = { InstrType::LDX, AddrMode::Immediate, 2, 2 };
//	lookup[0xA6] = { InstrType::LDX, AddrMode::ZeroPage, 2, 3 };
//	lookup[0xB6] = { InstrType::LDX, AddrMode::ZeroPageY, 2, 4 };
//	lookup[0xAE] = { InstrType::LDX, AddrMode::Absolute, 3, 4 };
//	lookup[0xBE] = { InstrType::LDX, AddrMode::AbsoluteY, 3, 4 };
//
//	lookup[0xA0] = { InstrType::LDY, AddrMode::Immediate, 2, 2 };
//	lookup[0xA4] = { InstrType::LDY, AddrMode::ZeroPage, 2, 3 };
//	lookup[0xB4] = { InstrType::LDY, AddrMode::ZeroPageX, 2, 4 };
//	lookup[0xAC] = { InstrType::LDY, AddrMode::Absolute, 3, 4 };
//	lookup[0xBC] = { InstrType::LDY, AddrMode::AbsoluteX, 3, 4 };
//
//	lookup[0x4A] = { InstrType::LSR, AddrMode::Accumulator, 1, 2 };
//	lookup[0x46] = { InstrType::LSR, AddrMode::ZeroPage, 2, 5 };
//	lookup[0x56] = { InstrType::LSR, AddrMode::ZeroPageX, 2, 6 };
//	lookup[0x4E] = { InstrType::LSR, AddrMode::Absolute, 3, 6 };
//	lookup[0x5E] = { InstrType::LSR, AddrMode::AbsoluteX, 3, 7 };
//
//	lookup[0xEA] = { InstrType::NOP, AddrMode::Implicit, 1, 2 };
//
//	lookup[0x09] = { InstrType::ORA, AddrMode::Immediate, 2, 2 };
//	lookup[0x05] = { InstrType::ORA, AddrMode::ZeroPage, 2, 3 };
//	lookup[0x15] = { InstrType::ORA, AddrMode::ZeroPageX, 2, 4 };
//	lookup[0x0D] = { InstrType::ORA, AddrMode::Absolute, 3, 4 };
//	lookup[0x1D] = { InstrType::ORA, AddrMode::AbsoluteX, 3, 4 };
//	lookup[0x19] = { InstrType::ORA, AddrMode::AbsoluteY, 3, 4 };
//	lookup[0x01] = { InstrType::ORA, AddrMode::IndexedIndirect, 2, 6 };
//	lookup[0x11] = { InstrType::ORA, AddrMode::IndirectIndexed, 2, 5 };
//
//	lookup[0x48] = { InstrType::PHA, AddrMode::Implicit, 1, 3 };
//
//	lookup[0x08] = { InstrType::PHP, AddrMode::Implicit, 1, 3 };
//
//	lookup[0x68] = { InstrType::PLA, AddrMode::Implicit, 1, 4 };
//
//	lookup[0x28] = { InstrType::PLP, AddrMode::Implicit, 1, 4 };
//
//	lookup[0x2A] = { InstrType::ROL, AddrMode::Accumulator, 1, 2 };
//	lookup[0x26] = { InstrType::ROL, AddrMode::ZeroPage, 2, 5 };
//	lookup[0x36] = { InstrType::ROL, AddrMode::ZeroPageX, 2, 6 };
//	lookup[0x2E] = { InstrType::ROL, AddrMode::Absolute, 3, 6 };
//	lookup[0x3E] = { InstrType::ROL, AddrMode::AbsoluteX, 3, 7 };
//
//	lookup[0x6A] = { InstrType::ROR, AddrMode::Accumulator, 1, 2 };
//	lookup[0x66] = { InstrType::ROR, AddrMode::ZeroPage, 2, 5 };
//	lookup[0x76] = { InstrType::ROR, AddrMode::ZeroPageX, 2, 6 };
//	lookup[0x6E] = { InstrType::ROR, AddrMode::Absolute, 3, 6 };
//	lookup[0x7E] = { InstrType::ROR, AddrMode::AbsoluteX, 3, 7 };
//
//	lookup[0x40] = { InstrType::RTI, AddrMode::Implicit, 1, 6 };
//
//	lookup[0x60] = { InstrType::RTS, AddrMode::Implicit, 1, 6 };
//
//	lookup[0xE9] = { InstrType::SBC, AddrMode::Immediate, 2, 2 };
//	lookup[0xE5] = { InstrType::SBC, AddrMode::ZeroPage, 2, 3 };
//	lookup[0xF5] = { InstrType::SBC, AddrMode::ZeroPageX, 2, 4 };
//	lookup[0xED] = { InstrType::SBC, AddrMode::Absolute, 3, 4 };
//	lookup[0xFD] = { InstrType::SBC, AddrMode::AbsoluteX, 3, 4 };
//	lookup[0xF9] = { InstrType::SBC, AddrMode::AbsoluteY, 3, 4 };
//	lookup[0xE1] = { InstrType::SBC, AddrMode::IndexedIndirect, 2, 6 };
//	lookup[0xF1] = { InstrType::SBC, AddrMode::IndirectIndexed, 2, 5 };
//
//	lookup[0x38] = { InstrType::SEC, AddrMode::Implicit, 1, 2 };
//
//	lookup[0xF8] = { InstrType::SED, AddrMode::Implicit, 1, 2 };
//
//	lookup[0x78] = { InstrType::SEI, AddrMode::Implicit, 1, 2 };
//
//	lookup[0x85] = { InstrType::STA, AddrMode::ZeroPage, 2, 3 };
//	lookup[0x95] = { InstrType::STA, AddrMode::ZeroPageX, 2, 4 };
//	lookup[0x8D] = { InstrType::STA, AddrMode::Absolute, 3, 4 };
//	lookup[0x9D] = { InstrType::STA, AddrMode::AbsoluteX, 3, 5 };
//	lookup[0x99] = { InstrType::STA, AddrMode::AbsoluteY, 3, 5 };
//	lookup[0x81] = { InstrType::STA, AddrMode::IndexedIndirect, 2, 6 };
//	lookup[0x91] = { InstrType::STA, AddrMode::IndirectIndexed, 2, 6 };
//
//	lookup[0x86] = { InstrType::STX, AddrMode::ZeroPage, 2, 3 };
//	lookup[0x96] = { InstrType::STX, AddrMode::ZeroPageY, 2, 4 };
//	lookup[0x8E] = { InstrType::STX, AddrMode::Absolute, 3, 4 };
//
//	lookup[0x84] = { InstrType::STY, AddrMode::ZeroPage, 2, 3 };
//	lookup[0x94] = { InstrType::STY, AddrMode::ZeroPageX, 2, 4 };
//	lookup[0x8C] = { InstrType::STY, AddrMode::Absolute, 3, 4 };
//
//	lookup[0xAA] = { InstrType::TAX, AddrMode::Implicit, 1, 2 };
//
//	lookup[0xA8] = { InstrType::TAY, AddrMode::Implicit, 1, 2 };
//
//	lookup[0xBA] = { InstrType::TSX, AddrMode::Implicit, 1, 2 };
//
//	lookup[0x8A] = { InstrType::TXA, AddrMode::Implicit, 1, 2 };
//
//	lookup[0x9A] = { InstrType::TXS, AddrMode::Implicit, 1, 2 };
//
//	lookup[0x98] = { InstrType::TYA, AddrMode::Implicit, 1, 2 };
//
//	return lookup;
//}();
//
//CPU::CPU() : CPU{nullptr}
//{
//
//}
//
//CPU::CPU(CPUBus* bus) : m_Bus{bus}
//{
//	m_Regs.A = 0;
//	m_Regs.X = 0;
//	m_Regs.Y = 0;
//	m_Regs.PC = 0;
//	m_Regs.S = 0xFD;
//	m_StatusReg = StatusFlag::InterruptDisable | StatusFlag::Unused;
//}
//
//void CPU::TriggerNMI()
//{
//	m_NmiPending = true;
//}
//
//void CPU::Reset()
//{
//	m_Regs.PC = ReadWord(RESET_VECTOR);
//	m_Regs.S = 0xFD;
//	m_StatusReg.Set(StatusFlag::InterruptDisable);
//	m_NextFetchCycle = 7;
//}
//
//void CPU::PerformCycle()
//{
//	m_TotalCycles++;
//
//	if (m_TotalCycles < m_NextFetchCycle)
//	{
//		return;
//	}
//
//	if (m_NmiPending)
//	{
//		HandleInterrupt(InterruptType::NMI);
//		m_NextFetchCycle += 7;
//		m_NmiPending = false;
//		return;
//	}
//
//	m_Branched = false;
//	const u8 opCode = Read(m_Regs.PC);
//	
//	m_CurInstr = s_OpcodeLookup[opCode];
//	if (m_CurInstr.instrType == InstrType::None)
//	{
//		// Treat as NOP if illegal op for now
//		m_CurInstr = s_OpcodeLookup[0xEA];
//	}
//
//	//PrintState();
//
//	ExecuteInstruction();
//
//	if (!m_Branched)
//		m_Regs.PC += m_CurInstr.byteCount;
//}
//
//// TODO: fix side effects
//void CPU::PrintState() const
//{
//	char buf[128];
//
//	const int instrBytes = m_CurInstr.byteCount;
//
//	char addrBuf[32];
//
//	u16 operand = 0;
//	if (m_CurInstr.byteCount == 2)
//		operand = Read(m_Regs.PC + 1);
//	if (m_CurInstr.byteCount == 3)
//		operand = ReadWord(m_Regs.PC + 1);
//
//	const u16 addr = ResolveAddress(m_CurInstr.addrMode);
//
//	DebugUtils::AddrModeToStr(
//		m_CurInstr.addrMode,
//		operand,
//		addr,
//		0,
//		addrBuf,
//		sizeof(addrBuf));
//
//	const int n = std::snprintf(buf, sizeof(buf),
//		"%04X  %02X %02X %02X  %s %-32s A:%02X X:%02X Y:%02X P:%02X S:%02X CYC:%llu\n",
//		m_Regs.PC,
//		Read(m_Regs.PC),
//		instrBytes > 1 ? Read(m_Regs.PC + 1) : 0,
//		instrBytes > 2 ? Read(m_Regs.PC + 2) : 0,
//		DebugUtils::InstrTypeToStr(m_CurInstr.instrType),
//		addrBuf,
//		m_Regs.A, m_Regs.X, m_Regs.Y, m_StatusReg.GetRaw(), m_Regs.S,
//		m_TotalCycles
//	);
//	
//	if (instrBytes < 2)
//	{
//		buf[9] = ' ';
//		buf[10] = ' ';
//	}
//	if (instrBytes < 3)
//	{
//		buf[12] = ' ';
//		buf[13] = ' ';
//	}
//
//	std::fwrite(buf, 1, n, stdout);
//}
//
//void CPU::ExecuteInstruction()
//{
//	const InstructionFunc f = ResolveInstructionFunction(m_CurInstr.instrType);
//
//	const u8 addCycles = (this->*f)(m_CurInstr.addrMode);
//
//	m_NextFetchCycle += addCycles + m_CurInstr.cycleCount;
//}
//
//u8 CPU::Read(u16 addr) const
//{
//	return m_Bus->Read(addr);
//}
//
//u16 CPU::ReadWord(u16 addr) const
//{
//	const u8 low = m_Bus->Read(addr);
//	const u8 high = m_Bus->Read(addr + 1);
//	return low | (high << 8);
//}
//
//void CPU::Write(u16 addr, u8 val)
//{
//	if (addr == 0x4014)
//	{
//		m_NextFetchCycle += 513;
//	}
//
//	return m_Bus->Write(addr, val);
//}
//
//void CPU::WriteWord(u16 addr, u16 val)
//{
//	const u8 low = val & 0xFF;
//	const u8 high = (val >> 8) & 0xFF;
//	m_Bus->Write(addr, low);
//	m_Bus->Write(addr + 1, high);
//}
//
//void CPU::PushStack(u8 val)
//{
//	Write(m_Regs.S + STACK_BEGIN, val);
//	m_Regs.S--;
//}
//
//void CPU::PushStackWord(u16 val)
//{
//	const u8 low = val & 0xFF;
//	const u8 high = (val >> 8) & 0xFF;
//	PushStack(high);
//	PushStack(low);
//}
//
//u8 CPU::PopStack()
//{
//	m_Regs.S++;
//	return Read(m_Regs.S + STACK_BEGIN);
//}
//
//u16 CPU::PopStackWord()
//{
//	const u8 low = PopStack();
//	const u8 high = PopStack();
//	return low | (high << 8);
//}
//
//u8 CPU::PeekStack() const
//{
//	return Read(m_Regs.S + 1 + STACK_BEGIN);
//}
//
//u16 CPU::PeekStackWord() const
//{
//	const u8 low = Read(m_Regs.S + 1 + STACK_BEGIN);
//	const u8 high = Read(m_Regs.S + 2 + STACK_BEGIN);
//	return low | (high << 8);
//}
//
//// TODO: fix side effects
//bool CPU::AddsCycle(AddrMode addrMode) const
//{
//	switch (addrMode)
//	{
//	case AddrMode::AbsoluteX:
//	{
//		const u16 base = ReadWord(m_Regs.PC + 1);
//		const u16 addr = base + m_Regs.X;
//		return (base & 0xFF00) != (addr & 0xFF00);
//	}
//	case AddrMode::AbsoluteY:
//	{
//		const u16 base = ReadWord(m_Regs.PC + 1);
//		const u16 addr = base + m_Regs.Y;
//		return (base & 0xFF00) != (addr & 0xFF00);
//	}
//	case AddrMode::IndirectIndexed: 
//	{
//		const u8 arg = Read(m_Regs.PC + 1);
//		const u8 low = Read(arg);
//		const u8 high = Read((arg + 1) & 0xFF);
//		const u16 base = low | (high << 8);
//		const u16 addr = base + m_Regs.Y;
//		return (base & 0xFF00) != (addr & 0xFF00);
//	}
//	default:
//		return false;
//	}
//}
//
//u8 CPU::Branch(u16 addr)
//{
//	const u16 updated = addr;
//	u8 addCycles = 1;
//	if ((updated & 0xFF00) != ((m_Regs.PC + m_CurInstr.byteCount) & 0xFF00))
//		addCycles++;
//	m_Regs.PC = updated;
//	m_Branched = true;
//
//	return addCycles;
//}
//
//void CPU::HandleInterrupt(InterruptType interruptType)
//{
//	if (interruptType == InterruptType::IRQ && m_StatusReg.Test(StatusFlag::InterruptDisable))
//		return;
//
//	PushStackWord(m_Regs.PC);
//
//	u8 flags = m_StatusReg.GetRaw();
//	if (interruptType == InterruptType::BRK)
//		flags |= static_cast<u8>(StatusFlag::Break);
//
//	PushStack(flags);
//
//	m_StatusReg.Set(StatusFlag::InterruptDisable);
//
//	switch (interruptType)
//	{
//	case InterruptType::BRK:
//	case InterruptType::IRQ:
//		m_Regs.PC = ReadWord(IRQ_VECTOR);
//		break;
//	case InterruptType::NMI:
//		m_Regs.PC = ReadWord(NMI_VECTOR);
//		break;
//	}
//
//	m_Branched = true;
//}
//
//u16 CPU::ResolveAddress(AddrMode addrMode) const
//{
//	switch (addrMode)
//	{
//	case AddrMode::None: return 0;
//	case AddrMode::Implicit: return ResolveImplicit();
//	case AddrMode::Accumulator: return ResolveAccumulator();
//	case AddrMode::Immediate: return ResolveImmediate();
//	case AddrMode::ZeroPage: return ResolveZeroPage();
//	case AddrMode::ZeroPageX: return ResolveZeroPageX();
//	case AddrMode::ZeroPageY: return ResolveZeroPageY();
//	case AddrMode::Relative: return ResolveRelative();
//	case AddrMode::Absolute: return ResolveAbsolute();
//	case AddrMode::AbsoluteX: return ResolveAbsoluteX();
//	case AddrMode::AbsoluteY: return ResolveAbsoluteY();
//	case AddrMode::Indirect: return ResolveIndirect();
//	case AddrMode::IndexedIndirect: return ResolveIndexedIndirect();
//	case AddrMode::IndirectIndexed: return ResolveIndirectIndexed();
//	default: return 0;
//	}
//}
//
//InstructionFunc CPU::ResolveInstructionFunction(InstrType instrType) const
//{
//	switch (instrType)
//	{
//	case InstrType::None: return nullptr;
//	case InstrType::ADC:  return &CPU::ADC;
//	case InstrType::AND:  return &CPU::AND;
//	case InstrType::ASL:  return &CPU::ASL;
//	case InstrType::BCC:  return &CPU::BCC;
//	case InstrType::BCS:  return &CPU::BCS;
//	case InstrType::BEQ:  return &CPU::BEQ;
//	case InstrType::BIT:  return &CPU::BIT;
//	case InstrType::BMI:  return &CPU::BMI;
//	case InstrType::BNE:  return &CPU::BNE;
//	case InstrType::BPL:  return &CPU::BPL;
//	case InstrType::BRK:  return &CPU::BRK;
//	case InstrType::BVC:  return &CPU::BVC;
//	case InstrType::BVS:  return &CPU::BVS;
//	case InstrType::CLC:  return &CPU::CLC;
//	case InstrType::CLD:  return &CPU::CLD;
//	case InstrType::CLI:  return &CPU::CLI;
//	case InstrType::CLV:  return &CPU::CLV;
//	case InstrType::CMP:  return &CPU::CMP;
//	case InstrType::CPX:  return &CPU::CPX;
//	case InstrType::CPY:  return &CPU::CPY;
//	case InstrType::DEC:  return &CPU::DEC;
//	case InstrType::DEX:  return &CPU::DEX;
//	case InstrType::DEY:  return &CPU::DEY;
//	case InstrType::EOR:  return &CPU::EOR;
//	case InstrType::INC:  return &CPU::INC;
//	case InstrType::INX:  return &CPU::INX;
//	case InstrType::INY:  return &CPU::INY;
//	case InstrType::JMP:  return &CPU::JMP;
//	case InstrType::JSR:  return &CPU::JSR;
//	case InstrType::LDA:  return &CPU::LDA;
//	case InstrType::LDX:  return &CPU::LDX;
//	case InstrType::LDY:  return &CPU::LDY;
//	case InstrType::LSR:  return &CPU::LSR;
//	case InstrType::NOP:  return &CPU::NOP;
//	case InstrType::ORA:  return &CPU::ORA;
//	case InstrType::PHA:  return &CPU::PHA;
//	case InstrType::PHP:  return &CPU::PHP;
//	case InstrType::PLA:  return &CPU::PLA;
//	case InstrType::PLP:  return &CPU::PLP;
//	case InstrType::ROL:  return &CPU::ROL;
//	case InstrType::ROR:  return &CPU::ROR;
//	case InstrType::RTI:  return &CPU::RTI;
//	case InstrType::RTS:  return &CPU::RTS;
//	case InstrType::SBC:  return &CPU::SBC;
//	case InstrType::SEC:  return &CPU::SEC;
//	case InstrType::SED:  return &CPU::SED;
//	case InstrType::SEI:  return &CPU::SEI;
//	case InstrType::STA:  return &CPU::STA;
//	case InstrType::STX:  return &CPU::STX;
//	case InstrType::STY:  return &CPU::STY;
//	case InstrType::TAX:  return &CPU::TAX;
//	case InstrType::TAY:  return &CPU::TAY;
//	case InstrType::TSX:  return &CPU::TSX;
//	case InstrType::TXA:  return &CPU::TXA;
//	case InstrType::TXS:  return &CPU::TXS;
//	case InstrType::TYA:  return &CPU::TYA;
//	default: return nullptr;
//	}
//}
//
//u16 CPU::ResolveImplicit() const
//{
//	return 0;
//}
//
//u16 CPU::ResolveAccumulator() const
//{
//	return 0;
//}
//
//u16 CPU::ResolveImmediate() const
//{
//	return m_Regs.PC + 1;
//}
//
//u16 CPU::ResolveZeroPage() const
//{
//	return Read(m_Regs.PC + 1);
//}
//
//u16 CPU::ResolveZeroPageX() const
//{
//	return (Read(m_Regs.PC + 1) + m_Regs.X) & 0xFF;
//}
//
//u16 CPU::ResolveZeroPageY() const
//{
//	return (Read(m_Regs.PC + 1) + m_Regs.Y) & 0xFF;
//}
//
//u16 CPU::ResolveRelative() const
//{
//	u16 addr = Read(m_Regs.PC + 1);
//	if (addr & 0x80)
//	{
//		addr |= 0xFF00;
//	}
//	return addr + m_Regs.PC + 2;
//}
//
//u16 CPU::ResolveAbsolute() const
//{
//	return ReadWord(m_Regs.PC + 1);
//}
//
//u16 CPU::ResolveAbsoluteX() const
//{
//	return ReadWord(m_Regs.PC + 1) + m_Regs.X;
//}
//
//u16 CPU::ResolveAbsoluteY() const
//{
//	return ReadWord(m_Regs.PC + 1) + m_Regs.Y;
//}
//
//u16 CPU::ResolveIndirect() const
//{
//	const u16 addr = ReadWord(m_Regs.PC + 1);
//	const u8 low = Read(addr);
//	// Intentional bug
//	const u8 high = ((addr & 0xFF) == 0xFF) ? Read(addr & 0xFF00) : Read(addr + 1);
//	return low | (high << 8);
//}
//
//u16 CPU::ResolveIndexedIndirect() const
//{
//	const u8 arg = Read(m_Regs.PC + 1);
//	const u8 low = Read((arg + m_Regs.X) & 0xFF);
//	const u8 high = Read((arg + m_Regs.X + 1) & 0xFF);
//	return low | (high << 8);
//}
//
//u16 CPU::ResolveIndirectIndexed() const
//{
//	const u8 arg = Read(m_Regs.PC + 1);
//	const u8 low = Read(arg);
//	const u8 high = Read((arg + 1) & 0xFF);
//	const u16 baseAddr = low | (high << 8);
//	return baseAddr + m_Regs.Y;
//}
//
//u8 CPU::ADC(AddrMode addrMode) 
//{
//	const u8 val = Read(ResolveAddress(addrMode));
//	const u16 result = m_Regs.A + val + m_StatusReg.Test(StatusFlag::Carry);
//
//	m_StatusReg.Set(StatusFlag::Carry, result > 0xFF);
//	m_StatusReg.Set(StatusFlag::Zero, (result & 0xFF) == 0);
//	m_StatusReg.Set(StatusFlag::Overflow, (result ^ m_Regs.A) & (result ^ val) & 0x80);
//	m_StatusReg.Set(StatusFlag::Negative, result & 0x80);
//
//	m_Regs.A = result & 0xFF;
//
//	return AddsCycle(addrMode);
//}
//
//u8 CPU::AND(AddrMode addrMode)
//{
//	const u8 val = Read(ResolveAddress(addrMode));
//	//if (addrMode == AddrMode::Immediate && val == (1 << 6))
//	//{
//	//	__debugbreak();
//	//}
//	m_Regs.A &= val;
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
//
//	return AddsCycle(addrMode);
//}
//
//u8 CPU::ASL(AddrMode addrMode)
//{
//	if (addrMode == AddrMode::Accumulator)
//	{
//		m_StatusReg.Set(StatusFlag::Carry, m_Regs.A & 0x80);
//		m_Regs.A <<= 1;
//		m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
//		m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
//	}
//	else
//	{
//		const u16 addr = ResolveAddress(addrMode);
//		u8 val = Read(addr);
//		Write(addr, val);
//		m_StatusReg.Set(StatusFlag::Carry, val & 0x80);
//		val <<= 1;
//		m_StatusReg.Set(StatusFlag::Zero, val == 0);
//		m_StatusReg.Set(StatusFlag::Negative, val & 0x80);
//		Write(addr, val);
//	}
//	return 0;
//}
//
//u8 CPU::BCC(AddrMode addrMode)
//{
//	if (!m_StatusReg.Test(StatusFlag::Carry))
//		return Branch(ResolveAddress(addrMode));
//	return 0;
//}
//
//u8 CPU::BCS(AddrMode addrMode)
//{ 
//	if (m_StatusReg.Test(StatusFlag::Carry))
//		return Branch(ResolveAddress(addrMode));
//	return 0;
//}
//
//u8 CPU::BEQ(AddrMode addrMode)
//{
//	if (m_StatusReg.Test(StatusFlag::Zero))
//		return Branch(ResolveAddress(addrMode));
//	return 0;
//}
//
//u8 CPU::BIT(AddrMode addrMode)
//{
//	const u8 val = Read(ResolveAddress(addrMode));
//	const u8 result = m_Regs.A & val;
//	m_StatusReg.Set(StatusFlag::Zero, result == 0);
//	m_StatusReg.Set(StatusFlag::Overflow, val & 0x40);
//	m_StatusReg.Set(StatusFlag::Negative, val & 0x80);
//	return 0;
//}
//
//u8 CPU::BMI(AddrMode addrMode)
//{
//	if (m_StatusReg.Test(StatusFlag::Negative))
//		return Branch(ResolveAddress(addrMode));
//	return 0;
//}
//
//u8 CPU::BNE(AddrMode addrMode)
//{
//	if (!m_StatusReg.Test(StatusFlag::Zero))
//		return Branch(ResolveAddress(addrMode));
//	return 0;
//}
//
//u8 CPU::BPL(AddrMode addrMode)
//{
//	if (!m_StatusReg.Test(StatusFlag::Negative))
//		return Branch(ResolveAddress(addrMode));
//	return 0;
//}
//
//u8 CPU::BRK(AddrMode addrMode)
//{
//	m_Regs.PC += 2;
//	HandleInterrupt(InterruptType::BRK);
//	return 0;
//}
//
//u8 CPU::BVC(AddrMode addrMode)
//{
//	if (!m_StatusReg.Test(StatusFlag::Overflow))
//		return Branch(ResolveAddress(addrMode));
//	return 0;
//}
//
//u8 CPU::BVS(AddrMode addrMode)
//{
//	if (m_StatusReg.Test(StatusFlag::Overflow))
//		return Branch(ResolveAddress(addrMode));
//	return 0;
//}
//
//u8 CPU::CLC(AddrMode addrMode)
//{
//	m_StatusReg.Clear(StatusFlag::Carry);
//	return 0;
//}
//
//u8 CPU::CLD(AddrMode addrMode)
//{
//	m_StatusReg.Clear(StatusFlag::Decimal);
//	return 0;
//}
//
//u8 CPU::CLI(AddrMode addrMode)
//{
//	m_StatusReg.Set(StatusFlag::InterruptDisable, false);
//	//m_InterruptDisableDelay = INTERRUPT_DISABLE_DELAY_OFF;
//	return 0;
//}
//
//u8 CPU::CLV(AddrMode addrMode)
//{
//	m_StatusReg.Clear(StatusFlag::Overflow);
//	return 0;
//}
//
//u8 CPU::CMP(AddrMode addrMode)
//{
//	const u8 val = Read(ResolveAddress(addrMode));
//	m_StatusReg.Set(StatusFlag::Carry, m_Regs.A >= val);
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == val);
//	m_StatusReg.Set(StatusFlag::Negative, (m_Regs.A - val) & 0x80);
//	return AddsCycle(addrMode);
//}
//
//u8 CPU::CPX(AddrMode addrMode)
//{
//	const u8 val = Read(ResolveAddress(addrMode));
//	m_StatusReg.Set(StatusFlag::Carry, m_Regs.X >= val);
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.X == val);
//	m_StatusReg.Set(StatusFlag::Negative, (m_Regs.X - val) & 0x80);
//	return 0;
//}
//
//u8 CPU::CPY(AddrMode addrMode)
//{
//	const u8 val = Read(ResolveAddress(addrMode));
//	m_StatusReg.Set(StatusFlag::Carry, m_Regs.Y >= val);
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.Y == val);
//	m_StatusReg.Set(StatusFlag::Negative, (m_Regs.Y - val) & 0x80);
//	return 0;
//}
//
//u8 CPU::DEC(AddrMode addrMode)
//{
//	const u16 addr = ResolveAddress(addrMode);
//	u8 val = Read(addr);
//	Write(addr, val);
//	val -= 1;
//	m_StatusReg.Set(StatusFlag::Zero, val == 0);
//	m_StatusReg.Set(StatusFlag::Negative, val & 0x80);
//	Write(addr, val);
//	return 0;
//}
//
//u8 CPU::DEX(AddrMode addrMode)
//{
//	m_Regs.X--;
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.X == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.X & 0x80);
//	return 0;
//}
//
//u8 CPU::DEY(AddrMode addrMode)
//{
//	m_Regs.Y--;
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.Y == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.Y & 0x80);
//	return 0;
//}
//
//u8 CPU::EOR(AddrMode addrMode)
//{
//	const u8 val = Read(ResolveAddress(addrMode));
//	m_Regs.A ^= val;
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
//	return AddsCycle(addrMode);
//}
//
//u8 CPU::INC(AddrMode addrMode)
//{
//	const u16 addr = ResolveAddress(addrMode);
//	u8 val = Read(addr);
//	Write(addr, val);
//	val++;
//	m_StatusReg.Set(StatusFlag::Zero, val == 0);
//	m_StatusReg.Set(StatusFlag::Negative, val & 0x80);
//	Write(addr, val);
//	return 0;
//}
//
//u8 CPU::INX(AddrMode addrMode)
//{
//	m_Regs.X++;
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.X == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.X & 0x80);
//	return 0;
//}
//
//u8 CPU::INY(AddrMode addrMode)
//{
//	m_Regs.Y++;
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.Y == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.Y & 0x80);
//	return 0;
//}
//
//u8 CPU::JMP(AddrMode addrMode)
//{
//	m_Regs.PC = ResolveAddress(addrMode);
//	m_Branched = true;
//	return 0;
//}
//
//u8 CPU::JSR(AddrMode addrMode)
//{
//	PushStackWord(m_Regs.PC + 2);
//	m_Regs.PC = ResolveAddress(addrMode);
//	m_Branched = true;
//	return 0;
//}
//
//u8 CPU::LDA(AddrMode addrMode)
//{
//	m_Regs.A = Read(ResolveAddress(addrMode));
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
//	return AddsCycle(addrMode);
//}
//
//u8 CPU::LDX(AddrMode addrMode)
//{
//	m_Regs.X = Read(ResolveAddress(addrMode));
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.X == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.X & 0x80);
//	return AddsCycle(addrMode);
//}
//
//u8 CPU::LDY(AddrMode addrMode)
//{
//	m_Regs.Y = Read(ResolveAddress(addrMode));
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.Y == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.Y & 0x80);
//	return AddsCycle(addrMode);
//}
//
//u8 CPU::LSR(AddrMode addrMode)
//{
//	if (addrMode == AddrMode::Accumulator)
//	{
//		m_StatusReg.Set(StatusFlag::Carry, m_Regs.A & 0x1);
//		m_Regs.A >>= 1;
//		m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
//		m_StatusReg.Clear(StatusFlag::Negative);
//	}
//	else
//	{
//		const u16 addr = ResolveAddress(addrMode);
//		u8 val = Read(addr);
//		Write(addr, val);
//		m_StatusReg.Set(StatusFlag::Carry, val & 0x1);
//		val >>= 1;
//		m_StatusReg.Set(StatusFlag::Zero, val == 0);
//		m_StatusReg.Clear(StatusFlag::Negative);
//		Write(addr, val);
//	}
//	return 0;
//}
//
//u8 CPU::NOP(AddrMode addrMode)
//{
//	return 0;
//}
//
//u8 CPU::ORA(AddrMode addrMode)
//{
//	const u8 val = Read(ResolveAddress(addrMode));
//	m_Regs.A |= val;
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
//	return AddsCycle(addrMode);
//}
//
//u8 CPU::PHA(AddrMode addrMode)
//{
//	PushStack(m_Regs.A);
//	return 0;
//}
//
//u8 CPU::PHP(AddrMode addrMode)
//{
//	PushStack((m_StatusReg | StatusFlag::Break).GetRaw());
//	return 0;
//}
//
//u8 CPU::PLA(AddrMode addrMode)
//{
//	m_Regs.A = PopStack();
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
//	return 0;
//}
//
//u8 CPU::PLP(AddrMode addrMode)
//{
//	const StatusRegister flags{ PopStack() };
//	//const bool interruptDisable = flags & StatusFlag::InterruptDisable;
//	m_StatusReg
//		.Set(StatusFlag::Carry, flags & StatusFlag::Carry)
//		.Set(StatusFlag::Zero, flags & StatusFlag::Zero)
//		.Set(StatusFlag::InterruptDisable, flags & StatusFlag::InterruptDisable)
//		.Set(StatusFlag::Decimal, flags & StatusFlag::Decimal)
//		.Set(StatusFlag::Overflow, flags & StatusFlag::Overflow)
//		.Set(StatusFlag::Negative, flags & StatusFlag::Negative);
//	//m_InterruptDisableDelay = interruptDisable ? INTERRUPT_DISABLE_DELAY_ON : INTERRUPT_DISABLE_DELAY_OFF;
//	return 0;
//}
//
//u8 CPU::ROL(AddrMode addrMode)
//{
//	const u16 addr = ResolveAddress(addrMode);
//	const u8 val = (addrMode == AddrMode::Accumulator) ? m_Regs.A : Read(addr);
//	const u8 res = (val << 1) | (m_StatusReg.Test(StatusFlag::Carry) ? 1 : 0);
//	m_StatusReg.Set(StatusFlag::Carry, val & 0x80);
//	m_StatusReg.Set(StatusFlag::Zero, res == 0);
//	m_StatusReg.Set(StatusFlag::Negative, res & 0x80);
//
//	if (addrMode == AddrMode::Accumulator)
//	{
//		m_Regs.A = res;
//	}
//	else
//	{
//		Write(addr, val);
//		Write(addr, res);
//	}
//	return 0;
//}
//
//u8 CPU::ROR(AddrMode addrMode)
//{
//	const u16 addr = ResolveAddress(addrMode);
//	const u8 val = (addrMode == AddrMode::Accumulator) ? m_Regs.A : Read(addr);
//	const u8 res = (val >> 1) | (m_StatusReg.Test(StatusFlag::Carry) << 7);
//	m_StatusReg.Set(StatusFlag::Carry, val & 0x1);
//	m_StatusReg.Set(StatusFlag::Zero, res == 0);
//	m_StatusReg.Set(StatusFlag::Negative, res & 0x80);
//
//	if (addrMode == AddrMode::Accumulator)
//	{
//		m_Regs.A = res;
//	}
//	else
//	{
//		Write(addr, val);
//		Write(addr, res);
//	}
//
//	return 0;
//}
//
//u8 CPU::RTI(AddrMode addrMode)
//{
//	const StatusRegister flags{ PopStack() };
//	m_StatusReg
//		.Set(StatusFlag::Carry, flags & StatusFlag::Carry)
//		.Set(StatusFlag::Zero, flags & StatusFlag::Zero)
//		.Set(StatusFlag::InterruptDisable, flags & StatusFlag::InterruptDisable)
//		.Set(StatusFlag::Decimal, flags & StatusFlag::Decimal)
//		.Set(StatusFlag::Overflow, flags & StatusFlag::Overflow)
//		.Set(StatusFlag::Negative, flags & StatusFlag::Negative);
//
//	m_Regs.PC = PopStackWord();
//	m_Branched = true;
//	return 0;
//}
//
//u8 CPU::RTS(AddrMode addrMode)
//{
//	m_Regs.PC = PopStackWord() + 1;
//	m_Branched = true;
//	return 0;
//}
//
//u8 CPU::SBC(AddrMode addrMode)
//{
//	const u8 val = ~Read(ResolveAddress(addrMode));
//	const u16 result = m_Regs.A + val + m_StatusReg.Test(StatusFlag::Carry);
//
//	m_StatusReg.Set(StatusFlag::Carry, result > 0xFF);
//	m_StatusReg.Set(StatusFlag::Zero, (result & 0xFF) == 0);
//	m_StatusReg.Set(StatusFlag::Overflow, (result ^ m_Regs.A) & (result ^ val) & 0x80);
//	m_StatusReg.Set(StatusFlag::Negative, result & 0x80);
//
//	m_Regs.A = result & 0xFF;
//
//	return AddsCycle(addrMode);
//}
//
//u8 CPU::SEC(AddrMode addrMode)
//{
//	m_StatusReg.Set(StatusFlag::Carry);
//	return 0;
//}
//
//u8 CPU::SED(AddrMode addrMode)
//{
//	m_StatusReg.Set(StatusFlag::Decimal);
//	return 0;
//}
//
//u8 CPU::SEI(AddrMode addrMode)
//{
//	m_StatusReg.Set(StatusFlag::InterruptDisable);
//	//m_InterruptDisableDelay = INTERRUPT_DISABLE_DELAY_ON;
//	return 0;
//}
//
//u8 CPU::STA(AddrMode addrMode)
//{
//	Write(ResolveAddress(addrMode), m_Regs.A);
//	return 0;
//}
//
//u8 CPU::STX(AddrMode addrMode)
//{
//	Write(ResolveAddress(addrMode), m_Regs.X);
//	return 0;
//}
//
//u8 CPU::STY(AddrMode addrMode)
//{
//	Write(ResolveAddress(addrMode), m_Regs.Y);
//	return 0;
//}
//
//u8 CPU::TAX(AddrMode addrMode)
//{
//	m_Regs.X = m_Regs.A;
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.X == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.X & 0x80);
//	return 0;
//}
//
//u8 CPU::TAY(AddrMode addrMode)
//{
//	m_Regs.Y = m_Regs.A;
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.Y == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.Y & 0x80);
//	return 0;
//}
//
//u8 CPU::TSX(AddrMode addrMode)
//{
//	m_Regs.X = m_Regs.S;
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.X == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.X & 0x80);
//	return 0;
//}
//
//u8 CPU::TXA(AddrMode addrMode)
//{
//	m_Regs.A = m_Regs.X;
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
//	return 0;
//}
//
//u8 CPU::TXS(AddrMode addrMode)
//{
//	m_Regs.S = m_Regs.X;
//	return 0;
//}
//
//u8 CPU::TYA(AddrMode addrMode)
//{
//	m_Regs.A = m_Regs.Y;
//	m_StatusReg.Set(StatusFlag::Zero, m_Regs.A == 0);
//	m_StatusReg.Set(StatusFlag::Negative, m_Regs.A & 0x80);
//	return 0;
//}