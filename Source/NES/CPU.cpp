#include "CPU.h"

#include "CPUBus.h"
#include "DebugUtils.h"

const Array<Instruction, 256> CPU::s_OpcodeLookup = [] {
	Array<Instruction, 256> lookup{};

	lookup[0x69] = { Op::ADC, InstrType::Immediate };
	lookup[0x65] = { Op::ADC, InstrType::ZeroPageRead };
	lookup[0x75] = { Op::ADC, InstrType::ZeroPageXRead };
	lookup[0x6D] = { Op::ADC, InstrType::AbsoluteRead };
	lookup[0x7D] = { Op::ADC, InstrType::AbsoluteXRead };
	lookup[0x79] = { Op::ADC, InstrType::AbsoluteYRead };
	lookup[0x61] = { Op::ADC, InstrType::IndexedIndirectRead };
	lookup[0x71] = { Op::ADC, InstrType::IndirectIndexedRead };

	lookup[0x29] = { Op::AND, InstrType::Immediate };
	lookup[0x25] = { Op::AND, InstrType::ZeroPageRead };
	lookup[0x35] = { Op::AND, InstrType::ZeroPageXRead };
	lookup[0x2D] = { Op::AND, InstrType::AbsoluteRead };
	lookup[0x3D] = { Op::AND, InstrType::AbsoluteXRead };
	lookup[0x39] = { Op::AND, InstrType::AbsoluteYRead };
	lookup[0x21] = { Op::AND, InstrType::IndexedIndirectRead };
	lookup[0x31] = { Op::AND, InstrType::IndirectIndexedRead };

	lookup[0x0A] = { Op::ASL, InstrType::Accumulator };
	lookup[0x06] = { Op::ASL, InstrType::ZeroPageReadModifyWrite };
	lookup[0x16] = { Op::ASL, InstrType::ZeroPageXReadModifyWrite };
	lookup[0x0E] = { Op::ASL, InstrType::AbsoluteReadModifyWrite };
	lookup[0x1E] = { Op::ASL, InstrType::AbsoluteXReadModifyWrite };

	lookup[0x90] = { Op::BCC, InstrType::Relative };
	lookup[0xB0] = { Op::BCS, InstrType::Relative };
	lookup[0xF0] = { Op::BEQ, InstrType::Relative };

	lookup[0x24] = { Op::BIT, InstrType::ZeroPageRead };
	lookup[0x2C] = { Op::BIT, InstrType::AbsoluteRead };

	lookup[0x30] = { Op::BMI, InstrType::Relative };
	lookup[0xD0] = { Op::BNE, InstrType::Relative };
	lookup[0x10] = { Op::BPL, InstrType::Relative };

	lookup[0x00] = { Op::BRK, InstrType::Interrupt };

	lookup[0x50] = { Op::BVC, InstrType::Relative };
	lookup[0x70] = { Op::BVS, InstrType::Relative };

	lookup[0x18] = { Op::CLC, InstrType::Implied };
	lookup[0xD8] = { Op::CLD, InstrType::Implied };
	lookup[0x58] = { Op::CLI, InstrType::Implied };
	lookup[0xB8] = { Op::CLV, InstrType::Implied };

	lookup[0xC9] = { Op::CMP, InstrType::Immediate };
	lookup[0xC5] = { Op::CMP, InstrType::ZeroPageRead };
	lookup[0xD5] = { Op::CMP, InstrType::ZeroPageXRead };
	lookup[0xCD] = { Op::CMP, InstrType::AbsoluteRead };
	lookup[0xDD] = { Op::CMP, InstrType::AbsoluteXRead };
	lookup[0xD9] = { Op::CMP, InstrType::AbsoluteYRead };
	lookup[0xC1] = { Op::CMP, InstrType::IndexedIndirectRead };
	lookup[0xD1] = { Op::CMP, InstrType::IndirectIndexedRead };

	lookup[0xE0] = { Op::CPX, InstrType::Immediate };
	lookup[0xE4] = { Op::CPX, InstrType::ZeroPageRead };
	lookup[0xEC] = { Op::CPX, InstrType::AbsoluteRead };

	lookup[0xC0] = { Op::CPY, InstrType::Immediate };
	lookup[0xC4] = { Op::CPY, InstrType::ZeroPageRead };
	lookup[0xCC] = { Op::CPY, InstrType::AbsoluteRead };

	lookup[0xC6] = { Op::DEC, InstrType::ZeroPageReadModifyWrite };
	lookup[0xD6] = { Op::DEC, InstrType::ZeroPageXReadModifyWrite };
	lookup[0xCE] = { Op::DEC, InstrType::AbsoluteReadModifyWrite };
	lookup[0xDE] = { Op::DEC, InstrType::AbsoluteXReadModifyWrite };

	lookup[0xCA] = { Op::DEX, InstrType::Implied };
	lookup[0x88] = { Op::DEY, InstrType::Implied };

	lookup[0x49] = { Op::EOR, InstrType::Immediate };
	lookup[0x45] = { Op::EOR, InstrType::ZeroPageRead };
	lookup[0x55] = { Op::EOR, InstrType::ZeroPageXRead };
	lookup[0x4D] = { Op::EOR, InstrType::AbsoluteRead };
	lookup[0x5D] = { Op::EOR, InstrType::AbsoluteXRead };
	lookup[0x59] = { Op::EOR, InstrType::AbsoluteYRead };
	lookup[0x41] = { Op::EOR, InstrType::IndexedIndirectRead };
	lookup[0x51] = { Op::EOR, InstrType::IndirectIndexedRead };

	lookup[0xE6] = { Op::INC, InstrType::ZeroPageReadModifyWrite };
	lookup[0xF6] = { Op::INC, InstrType::ZeroPageXReadModifyWrite };
	lookup[0xEE] = { Op::INC, InstrType::AbsoluteReadModifyWrite };
	lookup[0xFE] = { Op::INC, InstrType::AbsoluteXReadModifyWrite };

	lookup[0xE8] = { Op::INX, InstrType::Implied };
	lookup[0xC8] = { Op::INY, InstrType::Implied };

	lookup[0x4C] = { Op::JMP, InstrType::AbsoluteJMP };
	lookup[0x6C] = { Op::JMP, InstrType::AbsoluteIndirect };

	lookup[0x20] = { Op::JSR, InstrType::JSR };

	lookup[0xA9] = { Op::LDA, InstrType::Immediate };
	lookup[0xA5] = { Op::LDA, InstrType::ZeroPageRead };
	lookup[0xB5] = { Op::LDA, InstrType::ZeroPageXRead };
	lookup[0xAD] = { Op::LDA, InstrType::AbsoluteRead };
	lookup[0xBD] = { Op::LDA, InstrType::AbsoluteXRead };
	lookup[0xB9] = { Op::LDA, InstrType::AbsoluteYRead };
	lookup[0xA1] = { Op::LDA, InstrType::IndexedIndirectRead };
	lookup[0xB1] = { Op::LDA, InstrType::IndirectIndexedRead };

	lookup[0xA2] = { Op::LDX, InstrType::Immediate };
	lookup[0xA6] = { Op::LDX, InstrType::ZeroPageRead };
	lookup[0xB6] = { Op::LDX, InstrType::ZeroPageYRead };
	lookup[0xAE] = { Op::LDX, InstrType::AbsoluteRead };
	lookup[0xBE] = { Op::LDX, InstrType::AbsoluteYRead };

	lookup[0xA0] = { Op::LDY, InstrType::Immediate };
	lookup[0xA4] = { Op::LDY, InstrType::ZeroPageRead };
	lookup[0xB4] = { Op::LDY, InstrType::ZeroPageXRead };
	lookup[0xAC] = { Op::LDY, InstrType::AbsoluteRead };
	lookup[0xBC] = { Op::LDY, InstrType::AbsoluteXRead };

	lookup[0x4A] = { Op::LSR, InstrType::Accumulator };
	lookup[0x46] = { Op::LSR, InstrType::ZeroPageReadModifyWrite };
	lookup[0x56] = { Op::LSR, InstrType::ZeroPageXReadModifyWrite };
	lookup[0x4E] = { Op::LSR, InstrType::AbsoluteReadModifyWrite };
	lookup[0x5E] = { Op::LSR, InstrType::AbsoluteXReadModifyWrite };

	lookup[0xEA] = { Op::NOP, InstrType::Implied };

	lookup[0x09] = { Op::ORA, InstrType::Immediate };
	lookup[0x05] = { Op::ORA, InstrType::ZeroPageRead };
	lookup[0x15] = { Op::ORA, InstrType::ZeroPageXRead };
	lookup[0x0D] = { Op::ORA, InstrType::AbsoluteRead };
	lookup[0x1D] = { Op::ORA, InstrType::AbsoluteXRead };
	lookup[0x19] = { Op::ORA, InstrType::AbsoluteYRead };
	lookup[0x01] = { Op::ORA, InstrType::IndexedIndirectRead };
	lookup[0x11] = { Op::ORA, InstrType::IndirectIndexedRead };

	lookup[0x48] = { Op::PHA, InstrType::PHA };
	lookup[0x08] = { Op::PHP, InstrType::PHP };
	lookup[0x68] = { Op::PLA, InstrType::PLA };
	lookup[0x28] = { Op::PLP, InstrType::PLP };

	lookup[0x2A] = { Op::ROL, InstrType::Accumulator };
	lookup[0x26] = { Op::ROL, InstrType::ZeroPageReadModifyWrite };
	lookup[0x36] = { Op::ROL, InstrType::ZeroPageXReadModifyWrite };
	lookup[0x2E] = { Op::ROL, InstrType::AbsoluteReadModifyWrite };
	lookup[0x3E] = { Op::ROL, InstrType::AbsoluteXReadModifyWrite };

	lookup[0x6A] = { Op::ROR, InstrType::Accumulator };
	lookup[0x66] = { Op::ROR, InstrType::ZeroPageReadModifyWrite };
	lookup[0x76] = { Op::ROR, InstrType::ZeroPageXReadModifyWrite };
	lookup[0x6E] = { Op::ROR, InstrType::AbsoluteReadModifyWrite };
	lookup[0x7E] = { Op::ROR, InstrType::AbsoluteXReadModifyWrite };

	lookup[0x40] = { Op::RTI, InstrType::RTI };
	lookup[0x60] = { Op::RTS, InstrType::RTS };

	lookup[0xE9] = lookup[0xEB] = { Op::SBC, InstrType::Immediate };
	lookup[0xE5] = { Op::SBC, InstrType::ZeroPageRead };
	lookup[0xF5] = { Op::SBC, InstrType::ZeroPageXRead };
	lookup[0xED] = { Op::SBC, InstrType::AbsoluteRead };
	lookup[0xFD] = { Op::SBC, InstrType::AbsoluteXRead };
	lookup[0xF9] = { Op::SBC, InstrType::AbsoluteYRead };
	lookup[0xE1] = { Op::SBC, InstrType::IndexedIndirectRead };
	lookup[0xF1] = { Op::SBC, InstrType::IndirectIndexedRead };

	lookup[0x38] = { Op::SEC, InstrType::Implied };
	lookup[0xF8] = { Op::SED, InstrType::Implied };
	lookup[0x78] = { Op::SEI, InstrType::Implied };

	lookup[0x85] = { Op::STA, InstrType::ZeroPageWrite };
	lookup[0x95] = { Op::STA, InstrType::ZeroPageXWrite };
	lookup[0x8D] = { Op::STA, InstrType::AbsoluteWrite };
	lookup[0x9D] = { Op::STA, InstrType::AbsoluteXWrite };
	lookup[0x99] = { Op::STA, InstrType::AbsoluteYWrite };
	lookup[0x81] = { Op::STA, InstrType::IndexedIndirectWrite };
	lookup[0x91] = { Op::STA, InstrType::IndirectIndexedWrite };

	lookup[0x86] = { Op::STX, InstrType::ZeroPageWrite };
	lookup[0x96] = { Op::STX, InstrType::ZeroPageYWrite };
	lookup[0x8E] = { Op::STX, InstrType::AbsoluteWrite };

	lookup[0x84] = { Op::STY, InstrType::ZeroPageWrite };
	lookup[0x94] = { Op::STY, InstrType::ZeroPageXWrite };
	lookup[0x8C] = { Op::STY, InstrType::AbsoluteWrite };

	lookup[0xAA] = { Op::TAX, InstrType::Implied };
	lookup[0xA8] = { Op::TAY, InstrType::Implied };
	lookup[0xBA] = { Op::TSX, InstrType::Implied };
	lookup[0x8A] = { Op::TXA, InstrType::Implied };
	lookup[0x9A] = { Op::TXS, InstrType::Implied };
	lookup[0x98] = { Op::TYA, InstrType::Implied };

	// illegal ops
	lookup[0x4B] = { Op::ALR, InstrType::Immediate };

	lookup[0x0B] = lookup[0x2B] = { Op::ANC, InstrType::Immediate };

	lookup[0x8B] = { Op::ANE, InstrType::Immediate };

	lookup[0x6B] = { Op::ARR, InstrType::Immediate };

	lookup[0xC7] = { Op::DCP, InstrType::ZeroPageReadModifyWrite };
	lookup[0xD7] = { Op::DCP, InstrType::ZeroPageXReadModifyWrite };
	lookup[0xCF] = { Op::DCP, InstrType::AbsoluteReadModifyWrite };
	lookup[0xDF] = { Op::DCP, InstrType::AbsoluteXReadModifyWrite };
	lookup[0xDB] = { Op::DCP, InstrType::AbsoluteYReadModifyWrite };
	lookup[0xC3] = { Op::DCP, InstrType::IndexedIndirectReadModifyWrite };
	lookup[0xD3] = { Op::DCP, InstrType::IndirectIndexedReadModifyWrite };

	lookup[0xE7] = { Op::ISC, InstrType::ZeroPageReadModifyWrite };
	lookup[0xF7] = { Op::ISC, InstrType::ZeroPageXReadModifyWrite };
	lookup[0xEF] = { Op::ISC, InstrType::AbsoluteReadModifyWrite };
	lookup[0xFF] = { Op::ISC, InstrType::AbsoluteXReadModifyWrite };
	lookup[0xFB] = { Op::ISC, InstrType::AbsoluteYReadModifyWrite };
	lookup[0xE3] = { Op::ISC, InstrType::IndexedIndirectReadModifyWrite };
	lookup[0xF3] = { Op::ISC, InstrType::IndirectIndexedReadModifyWrite };

	lookup[0xBB] = { Op::LAS, InstrType::AbsoluteYRead };
	
	lookup[0xA7] = { Op::LAX, InstrType::ZeroPageRead };
	lookup[0xB7] = { Op::LAX, InstrType::ZeroPageYRead };
	lookup[0xAF] = { Op::LAX, InstrType::AbsoluteRead };
	lookup[0xBF] = { Op::LAX, InstrType::AbsoluteYRead };
	lookup[0xA3] = { Op::LAX, InstrType::IndexedIndirectRead };
	lookup[0xB3] = { Op::LAX, InstrType::IndirectIndexedRead };

	lookup[0xAB] = { Op::LXA, InstrType::Immediate };

	lookup[0x27] = { Op::RLA, InstrType::ZeroPageReadModifyWrite };
	lookup[0x37] = { Op::RLA, InstrType::ZeroPageXReadModifyWrite };
	lookup[0x2F] = { Op::RLA, InstrType::AbsoluteReadModifyWrite };
	lookup[0x3F] = { Op::RLA, InstrType::AbsoluteXReadModifyWrite };
	lookup[0x3B] = { Op::RLA, InstrType::AbsoluteYReadModifyWrite };
	lookup[0x23] = { Op::RLA, InstrType::IndexedIndirectReadModifyWrite };
	lookup[0x33] = { Op::RLA, InstrType::IndirectIndexedReadModifyWrite };

	lookup[0x67] = { Op::RRA, InstrType::ZeroPageReadModifyWrite };
	lookup[0x77] = { Op::RRA, InstrType::ZeroPageXReadModifyWrite };
	lookup[0x6F] = { Op::RRA, InstrType::AbsoluteReadModifyWrite };
	lookup[0x7F] = { Op::RRA, InstrType::AbsoluteXReadModifyWrite };
	lookup[0x7B] = { Op::RRA, InstrType::AbsoluteYReadModifyWrite };
	lookup[0x63] = { Op::RRA, InstrType::IndexedIndirectReadModifyWrite };
	lookup[0x73] = { Op::RRA, InstrType::IndirectIndexedReadModifyWrite };

	lookup[0x87] = { Op::SAX, InstrType::ZeroPageWrite };
	lookup[0x97] = { Op::SAX, InstrType::ZeroPageYWrite };
	lookup[0x8F] = { Op::SAX, InstrType::AbsoluteWrite };
	lookup[0x83] = { Op::SAX, InstrType::IndexedIndirectWrite };

	lookup[0xCB] = { Op::SBX, InstrType::Immediate };

	lookup[0x9F] = { Op::SHA, InstrType::AbsoluteYWrite };
	lookup[0x93] = { Op::SHA, InstrType::IndirectIndexedWrite };

	lookup[0x9E] = { Op::SHX, InstrType::AbsoluteYWrite };

	lookup[0x9C] = { Op::SHY, InstrType::AbsoluteXWrite };

	lookup[0x07] = { Op::SLO, InstrType::ZeroPageReadModifyWrite };
	lookup[0x17] = { Op::SLO, InstrType::ZeroPageXReadModifyWrite };
	lookup[0x0F] = { Op::SLO, InstrType::AbsoluteReadModifyWrite };
	lookup[0x1F] = { Op::SLO, InstrType::AbsoluteXReadModifyWrite };
	lookup[0x1B] = { Op::SLO, InstrType::AbsoluteYReadModifyWrite };
	lookup[0x03] = { Op::SLO, InstrType::IndexedIndirectReadModifyWrite };
	lookup[0x13] = { Op::SLO, InstrType::IndirectIndexedReadModifyWrite };

	lookup[0x47] = { Op::SRE, InstrType::ZeroPageReadModifyWrite };
	lookup[0x57] = { Op::SRE, InstrType::ZeroPageXReadModifyWrite };
	lookup[0x4F] = { Op::SRE, InstrType::AbsoluteReadModifyWrite };
	lookup[0x5F] = { Op::SRE, InstrType::AbsoluteXReadModifyWrite };
	lookup[0x5B] = { Op::SRE, InstrType::AbsoluteYReadModifyWrite };
	lookup[0x43] = { Op::SRE, InstrType::IndexedIndirectReadModifyWrite };
	lookup[0x53] = { Op::SRE, InstrType::IndirectIndexedReadModifyWrite };

	lookup[0x9B] = { Op::TAS, InstrType::AbsoluteYWrite };

	lookup[0x1A] = lookup[0x3A] = lookup[0x5A] = lookup[0x7A] = lookup[0xDA] = lookup[0xFA] =
	{ Op::NOP, InstrType::Implied };

	lookup[0x80] = lookup[0x82] = lookup[0x89] = lookup[0xC2] = lookup[0xE2] =
	{ Op::NOP, InstrType::Immediate };

	lookup[0x04] = lookup[0x44] = lookup[0x64] =
	{ Op::NOP, InstrType::ZeroPageRead };

	lookup[0x14] = lookup[0x34] = lookup[0x54] = lookup[0x74] = lookup[0xD4] = lookup[0xF4] =
	{ Op::NOP, InstrType::ZeroPageXRead };

	lookup[0x0C] = { Op::NOP, InstrType::AbsoluteRead };

	lookup[0x1C] = lookup[0x3C] = lookup[0x5C] = lookup[0x7C] = lookup[0xDC] = lookup[0xFC] =
	{ Op::NOP, InstrType::AbsoluteXRead };
	
	return lookup;
}();

enum StatusFlag
{
	STATUS_CARRY = 1 << 0,
	STATUS_ZERO = 1 << 1,
	STATUS_INTERRUPT_DISABLE = 1 << 2,
	STATUS_DECIMAL = 1 << 3,
	STATUS_BREAK = 1 << 4,
	STATUS_UNUSED = 1 << 5,
	STATUS_OVERFLOW = 1 << 6,
	STATUS_NEGATIVE = 1 << 7
};

static constexpr u16 STACK_BEGIN = 0x0100;

#define DONE() m_InstrDone = true

CPU::CPU() :
	CPU{nullptr}
{
	
}

CPU::CPU(CPUBus* bus) :
	m_Bus {bus}
{
	m_P = STATUS_INTERRUPT_DISABLE | STATUS_UNUSED;
	Reset();
}

void CPU::PerformCycle()
{
	m_TotalCycles++;

	if (m_DMAStatus == DMAStatus::Active)
	{
		DMAStep();
		return;
	}
	// DMA
	if (m_DMAStatus == DMAStatus::Scheduled && m_InstrCycle == 0)
	{

		// must begin reading on odd cycle, otherwise alignment cycle
		if (m_TotalCycles % 2 == 0)
		{
			m_DMAStatus = DMAStatus::Active;
		}
		// stall cycle at beginning of DMA
		return;
	}

	m_InstrCycle++;
	if (m_InstrCycle == 1)
	{
		// normal fetch
		if (m_CurInterrupt == InterruptType::None)
		{
			const u8 opcode = Read(m_PC++);
			m_CurInstr = s_OpcodeLookup[opcode];
			if (m_CurInstr.op == Op::None)
			{
				// Treat as NOP if illegal op for now
				m_CurInstr = s_OpcodeLookup[0xEA];
			}
			if (m_CurInstr.op == Op::BRK)
			{
				m_CurInterrupt = InterruptType::BRK;
			}
		}
		// interrupt handling
		else
		{
			m_CurInstr = { .op = Op::None, .type = InstrType::Interrupt };
		}

		return;
	}
	StepInstruction();

	if (m_InstrDone)
	{
		m_InstrDone = false;
		m_InstrCycle = 0;
		m_CurInterrupt = InterruptType::None;
		if (m_IRQPending)
		{
			m_CurInterrupt = InterruptType::IRQ;
		}
		if (m_NMIPending)
		{
			m_CurInterrupt = InterruptType::NMI;
			m_NMIPending = false;
		}
	}

	PollInterrupts();
}

void CPU::PollInterrupts()
{
	m_IRQPending = m_IRQLine && !(m_P & STATUS_INTERRUPT_DISABLE);
	if (!m_NMILinePrev && m_NMILine)
	{
		m_NMIPending = true;
	}
	m_NMILinePrev = m_NMILine;
}

void CPU::DMAStep()
{
	// read on odd cycles
	if (m_TotalCycles % 2 == 1)
	{
		m_Val = Read((m_DMAPage << 8) | m_DMAIndex);
	}
	// write on even cycles
	else
	{
		if (m_DMAIndex == 0xFF)
		{
			m_DMAStatus = DMAStatus::Inactive;
		}
		m_Bus->PPUDirectWrite(m_Val);
		m_DMAIndex++;
	}
}

void CPU::Reset()
{
	m_InstrCycle = 0;
	m_CurInterrupt = InterruptType::RES;
}

void CPU::SetNMILine(bool asserted)
{
	m_NMILine = asserted;
}

void CPU::SetIRQLine(bool asserted)
{
	m_IRQLine = asserted;
}

void CPU::PrintState()
{
	printf("%04X %s A:%02X X:%02X Y:%02X P:%02X S:%02X CYC:%llu\n", m_PC, DebugUtils::OpToStr(m_CurInstr.op), m_A, m_X, m_Y, m_P, m_S, m_TotalCycles + 6);
}

void CPU::StepInstruction()
{
	switch (m_CurInstr.type)
	{
	case InstrType::Interrupt: Interrupt(); break;
	case InstrType::RTI: RTI(); break;
	case InstrType::RTS: RTS(); break;
	case InstrType::PHA: PHA(); break;
	case InstrType::PHP: PHP(); break;
	case InstrType::PLA: PLA(); break;
	case InstrType::PLP: PLP(); break;
	case InstrType::JSR: JSR(); break;
	case InstrType::Accumulator: Accumulator(); break;
	case InstrType::Implied: Implied(); break;
	case InstrType::Immediate: Immediate(); break;
	case InstrType::AbsoluteJMP: AbsoluteJMP(); break;
	case InstrType::AbsoluteRead: AbsoluteRead(); break;
	case InstrType::AbsoluteReadModifyWrite: AbsoluteReadModifyWrite(); break;
	case InstrType::AbsoluteWrite: AbsoluteWrite(); break;
	case InstrType::ZeroPageRead: ZeroPageRead(); break;
	case InstrType::ZeroPageReadModifyWrite: ZeroPageReadModifyWrite(); break;
	case InstrType::ZeroPageWrite: ZeroPageWrite(); break;
	case InstrType::ZeroPageXRead: ZeroPageIndexedRead<IndexType::X>(); break;
	case InstrType::ZeroPageYRead: ZeroPageIndexedRead<IndexType::Y>(); break;
	case InstrType::ZeroPageXReadModifyWrite: ZeroPageXReadModifyWrite(); break;
	case InstrType::ZeroPageXWrite: ZeroPageIndexedWrite<IndexType::X>(); break;
	case InstrType::ZeroPageYWrite: ZeroPageIndexedWrite<IndexType::Y>(); break;
	case InstrType::AbsoluteXRead: AbsoluteIndexedRead<IndexType::X>(); break;
	case InstrType::AbsoluteYRead: AbsoluteIndexedRead<IndexType::Y>(); break;
	case InstrType::AbsoluteXReadModifyWrite: AbsoluteIndexedReadModifyWrite<IndexType::X>(); break;
	case InstrType::AbsoluteYReadModifyWrite: AbsoluteIndexedReadModifyWrite<IndexType::Y>(); break;
	case InstrType::AbsoluteXWrite: AbsoluteIndexedWrite<IndexType::X>(); break;
	case InstrType::AbsoluteYWrite: AbsoluteIndexedWrite<IndexType::Y>(); break;
	case InstrType::Relative: Relative(); break;
	case InstrType::IndexedIndirectRead: IndexedIndirectRead(); break;
	case InstrType::IndexedIndirectReadModifyWrite: IndexedIndirectReadModifyWrite(); break;
	case InstrType::IndexedIndirectWrite: IndexedIndirectWrite(); break;
	case InstrType::IndirectIndexedRead: IndirectIndexedRead(); break;
	case InstrType::IndirectIndexedReadModifyWrite: IndirectIndexedReadModifyWrite(); break;
	case InstrType::IndirectIndexedWrite: IndirectIndexedWrite(); break;
	case InstrType::AbsoluteIndirect: AbsoluteIndirect(); break;
	case InstrType::None:
	default:
		ASSERT(false);
		break;
	}
}

static constexpr u16 InterruptVectorLoc(InterruptType interrupt)
{
	switch (interrupt)
	{
	case InterruptType::NMI:
		return 0xFFFA;
	case InterruptType::RES:
		return 0xFFFC;
	case InterruptType::IRQ:
	case InterruptType::BRK:
		return 0xFFFE;
	default:
		ASSERT(false);
		return 0;
	}
}

void CPU::Interrupt()
{
	switch (m_InstrCycle)
	{
	case 2: 
		Read(m_PC); 
		if (m_CurInterrupt == InterruptType::BRK) 
			m_PC++;
		break;
	case 3: 
		// writes disabled for reset
		if (m_CurInterrupt != InterruptType::RES)
			Write(m_S + STACK_BEGIN, m_PC >> 8);
		m_S--;
		break;
	case 4: 
		// writes disabled for reset
		if (m_CurInterrupt != InterruptType::RES)
			Write(m_S + STACK_BEGIN, m_PC & 0xFF);
		m_S--;
		break;
	case 5:
		if (m_CurInterrupt != InterruptType::RES)
		{
			const u8 pushed = m_CurInterrupt == InterruptType::BRK ? m_P | STATUS_BREAK : m_P;
			Write(m_S + STACK_BEGIN, pushed);
		}
		m_S--;
		break;
	case 6: 
		m_ReadBuf[0] = Read(InterruptVectorLoc(m_CurInterrupt)); 
		break;
	case 7: 
		m_PC = (Read(InterruptVectorLoc(m_CurInterrupt) + 1) << 8) | m_ReadBuf[0]; 
		DONE(); 
		break;
	}
}

void CPU::RTI()
{
	switch (m_InstrCycle)
	{
	case 2: Read(m_PC); break;
	case 3: m_S++; break;
	case 4: m_P = (ReadStack() & ~STATUS_BREAK) | STATUS_UNUSED; m_S++; break;
	case 5: m_ReadBuf[0] = ReadStack(); m_S++; break;
	case 6: m_PC = (ReadStack() << 8) | m_ReadBuf[0]; DONE(); break;
	}
}

void CPU::RTS()
{
	switch (m_InstrCycle)
	{
	case 2: Read(m_PC); break;
	case 3: m_S++; break;
	case 4: m_ReadBuf[0] = ReadStack(); m_S++; break;
	case 5: m_PC = (ReadStack() << 8) | m_ReadBuf[0]; break;
	case 6: m_PC++; DONE(); break;
	}
}

void CPU::PHA()
{
	switch (m_InstrCycle)
	{
	case 2: Read(m_PC); break;
	case 3: PushStack(m_A); DONE(); break;
	}
}

void CPU::PHP()
{
	switch (m_InstrCycle)
	{
	case 2: Read(m_PC); break;
	case 3: PushStack(m_P | STATUS_BREAK); DONE(); break;
	}
}

void CPU::PLA()
{
	switch (m_InstrCycle)
	{
	case 2: Read(m_PC); break;
	case 3: m_S++; break;
	case 4: 
		m_A = ReadStack(); 
		SetStatusBit(STATUS_ZERO, m_A == 0); 
		SetStatusBit(STATUS_NEGATIVE, m_A & 0x80); 
		DONE();
		break;
	}
}

void CPU::PLP()
{
	switch (m_InstrCycle)
	{
	case 2: Read(m_PC); break;
	case 3: m_S++; break;
	case 4: m_P = (ReadStack() & ~STATUS_BREAK) | STATUS_UNUSED; DONE(); break;
	}
}

void CPU::JSR()
{
	switch (m_InstrCycle)
	{
	case 2: m_ReadBuf[0] = Read(m_PC++); break;
	case 3: break;
	case 4: PushStack(m_PC >> 8); break;
	case 5: PushStack(m_PC & 0xFF); break;
	case 6: m_PC = (Read(m_PC) << 8) | m_ReadBuf[0]; DONE(); break;
	}
}

void CPU::Accumulator()
{
	switch (m_InstrCycle)
	{
	case 2: Read(m_PC); m_Val = m_A; ExecuteOp(); m_A = m_Val; DONE(); break;
	}
}

void CPU::Implied()
{
	switch (m_InstrCycle)
	{
	case 2: Read(m_PC); ExecuteOp(); DONE(); break;
	}
}

void CPU::Immediate()
{
	switch (m_InstrCycle)
	{
	case 2: m_Val = Read(m_PC++); ExecuteOp(); DONE(); break;
	}
}

void CPU::AbsoluteJMP()
{
	switch (m_InstrCycle)
	{
	case 2: m_ReadBuf[0] = Read(m_PC++); break;
	case 3: m_PC = (Read(m_PC) << 8) | m_ReadBuf[0]; DONE(); break;
	}
}

void CPU::AbsoluteRead()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: m_Addr |= Read(m_PC++) << 8; break;
	case 4: m_Val = Read(m_Addr); ExecuteOp(); DONE(); break;
	}
}

void CPU::AbsoluteReadModifyWrite()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: m_Addr |= Read(m_PC++) << 8; break;
	case 4: m_Val = Read(m_Addr); break;
	case 5: Write(m_Addr, m_Val); ExecuteOp(); break;
	case 6: Write(m_Addr, m_Val); DONE(); break;
	}
}

void CPU::AbsoluteWrite()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: m_Addr |= Read(m_PC++) << 8; break;
	case 4: ExecuteOp(); Write(m_Addr, m_Val); DONE(); break;
	}
}

void CPU::ZeroPageRead()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: m_Val = Read(m_Addr); ExecuteOp(); DONE(); break;
	}
}

void CPU::ZeroPageReadModifyWrite()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: m_Val = Read(m_Addr); break;
	case 4: Write(m_Addr, m_Val); ExecuteOp(); break;
	case 5: Write(m_Addr, m_Val); DONE(); break;
	}
}

void CPU::ZeroPageWrite()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: ExecuteOp(); Write(m_Addr, m_Val); DONE(); break;
	}
}

template <IndexType IT>
void CPU::ZeroPageIndexedRead()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: Read(m_Addr); m_Addr = (m_Addr + (IT == IndexType::X ? m_X : m_Y)) & 0xFF; break;
	case 4: m_Val = Read(m_Addr); ExecuteOp(); DONE(); break;
	}
}

void CPU::ZeroPageXReadModifyWrite()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: Read(m_Addr); m_Addr = (m_Addr + m_X) & 0xFF; break;
	case 4: m_Val = Read(m_Addr); break;
	case 5: Write(m_Addr, m_Val); ExecuteOp(); break;
	case 6: Write(m_Addr, m_Val); DONE(); break;
	}
}

template <IndexType IT>
void CPU::ZeroPageIndexedWrite()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: Read(m_Addr); m_Addr = (m_Addr + (IT == IndexType::X ? m_X : m_Y)) & 0xFF; break;
	case 4: ExecuteOp(); Write(m_Addr, m_Val); DONE(); break;
	}
}

template <IndexType IT>
void CPU::AbsoluteIndexedRead()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3:
	{
		const u16 sumLow = m_Addr + (IT == IndexType::X ? m_X : m_Y);
		m_Addr = (Read(m_PC++) << 8) | (sumLow & 0xFF);
		m_Carry = sumLow >> 8;
		break;
	}
	case 4:
	{
		m_Val = Read(m_Addr);
		if (m_Carry)
		{
			m_Addr += 0x100;
		}
		else
		{
			ExecuteOp();
			DONE();
		}
		break;
	}
	case 5: m_Val = Read(m_Addr); ExecuteOp(); DONE(); break;
	}
}

template <IndexType IT>
void CPU::AbsoluteIndexedReadModifyWrite()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3:
	{
		const u16 sumLow = m_Addr + (IT == IndexType::X ? m_X : m_Y);
		m_Addr = (Read(m_PC++) << 8) | (sumLow & 0xFF);
		m_Carry = sumLow >> 8;
		break;
	}
	case 4: m_Val = Read(m_Addr); m_Addr += (m_Carry << 8); break;
	case 5: m_Val = Read(m_Addr); break;
	case 6: Write(m_Addr, m_Val); ExecuteOp(); break;
	case 7: Write(m_Addr, m_Val); DONE(); break;
	}
}

template <IndexType IT>
void CPU::AbsoluteIndexedWrite()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3:
	{
		const u16 sumLow = m_Addr + (IT == IndexType::X ? m_X : m_Y);
		m_Addr = (Read(m_PC++) << 8) | (sumLow & 0xFF);
		m_Carry = sumLow >> 8;
		break;
	}
	case 4: m_Val = Read(m_Addr); ExecuteOp(); m_Addr += (m_Carry << 8); break;
	case 5: Write(m_Addr, m_Val); DONE(); break;
	}
}

void CPU::Relative()
{
	switch (m_InstrCycle)
	{
	case 2: 
		m_Val = Read(m_PC++);
		ExecuteOp(); // will set m_BranchTaken
		if (!m_BranchTaken)
			DONE();
		break;
	// reached if branch occurs
	case 3:
	{
		Read(m_PC);
		const i16 offset = static_cast<i8>(m_Val);
		const u16 newPC = m_PC + offset;
		if ((m_PC & 0xFF00) == (newPC & 0xFF00))
		{
			m_PC = newPC;
			DONE();
		}
		break;
	}
	// reached if page crossed
	case 4:
	{
		const i16 offset = static_cast<i8>(m_Val);
		const u16 newPC = m_PC + offset;

		// read from wrong page first
		Read((m_PC & 0xFF00) | (newPC & 0xFF)); 
		
		m_PC = newPC;
		DONE();
		break;
	}
	}
}

void CPU::IndexedIndirectRead()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: Read(m_Addr); m_Addr = (m_Addr + m_X) & 0xFF; break;
	case 4: m_ReadBuf[0] = Read(m_Addr); break;
	case 5: m_Addr = (Read((m_Addr + 1) & 0xFF) << 8) | m_ReadBuf[0]; break;
	case 6: m_Val = Read(m_Addr); ExecuteOp(); DONE(); break;
	}
}

void CPU::IndexedIndirectReadModifyWrite()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: Read(m_Addr); m_Addr = (m_Addr + m_X) & 0xFF; break;
	case 4: m_ReadBuf[0] = Read(m_Addr); break;
	case 5: m_Addr = (Read((m_Addr + 1) & 0xFF) << 8) | m_ReadBuf[0]; break;
	case 6: m_Val = Read(m_Addr); break;
	case 7: Write(m_Addr, m_Val); ExecuteOp(); break;
	case 8: Write(m_Addr, m_Val); DONE(); break;
	}
}

void CPU::IndexedIndirectWrite()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: Read(m_Addr); m_Addr = (m_Addr + m_X) & 0xFF; break;
	case 4: m_ReadBuf[0] = Read(m_Addr); break;
	case 5: m_Addr = (Read((m_Addr + 1) & 0xFF) << 8) | m_ReadBuf[0]; break;
	case 6: ExecuteOp(); Write(m_Addr, m_Val); DONE(); break;
	}
}

void CPU::IndirectIndexedRead()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: m_ReadBuf[0] = Read(m_Addr); break;
	case 4:
	{
		const u16 sumLow = m_ReadBuf[0] + m_Y; 
		m_Addr = (Read((m_Addr + 1) & 0xFF) << 8) | (sumLow & 0xFF);
		m_Carry = sumLow >> 8;
		break;
	}
	case 5:
	{
		m_Val = Read(m_Addr);
		if (m_Carry)
		{
			m_Addr += 0x100;
		}
		else
		{
			ExecuteOp();
			DONE();
		}
		break;
	}
	case 6: m_Val = Read(m_Addr); ExecuteOp(); DONE(); break;
	}
}

void CPU::IndirectIndexedReadModifyWrite()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: m_ReadBuf[0] = Read(m_Addr); break;
	case 4:
	{
		const u16 sumLow = m_ReadBuf[0] + m_Y;
		m_Addr = (Read((m_Addr + 1) & 0xFF) << 8) | (sumLow & 0xFF);
		m_Carry = sumLow >> 8;
		break;
	}
	case 5: m_Val = Read(m_Addr); m_Addr += (m_Carry << 8); break;
	case 6: m_Val = Read(m_Addr); break;
	case 7: Write(m_Addr, m_Val); ExecuteOp(); break;
	case 8: Write(m_Addr, m_Val); DONE(); break;
	}
}

void CPU::IndirectIndexedWrite()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: m_ReadBuf[0] = Read(m_Addr); break;
	case 4: 
	{
		const u16 sumLow = m_ReadBuf[0] + m_Y;
		m_Addr = (Read((m_Addr + 1) & 0xFF) << 8) | (sumLow & 0xFF);
		m_Carry = sumLow >> 8;
		break;
	}
	case 5: Read(m_Addr); m_Addr += (m_Carry << 8); break;
	case 6: ExecuteOp(); Write(m_Addr, m_Val); DONE(); break;
	}
}

void CPU::AbsoluteIndirect()
{
	switch (m_InstrCycle)
	{
	case 2: m_Addr = Read(m_PC++); break;
	case 3: m_Addr = (Read(m_PC++) << 8) | m_Addr; break;
	case 4: m_ReadBuf[0] = Read(m_Addr); break;
	case 5:
	{
		// PC high fetched from same page as PC low
		const u16 adjusted = (m_Addr & 0xFF00) | ((m_Addr + 1) & 0xFF);
		m_PC = (Read(adjusted) << 8) | m_ReadBuf[0]; 
		DONE();
		break;
	}
	}
}

void CPU::PushStack(u8 val)
{
	Write(m_S + STACK_BEGIN, val);
	m_S--;
}

u8 CPU::PopStack()
{
	m_S++;
	return Read(m_S + STACK_BEGIN);
}

u8 CPU::ReadStack()
{
	return Read(m_S + STACK_BEGIN);
}

u8 CPU::Read(u16 addr)
{
	return m_Bus->Read(addr);
}

void CPU::Write(u16 addr, u8 val)
{
	m_Bus->Write(addr, val);
	if (addr == 0x4014)
	{
		m_DMAPage = val;
		m_DMAStatus = DMAStatus::Scheduled;
	}
}

void CPU::SetStatusBit(u8 mask, bool cond)
{
	m_P = (m_P & ~mask) | (cond ? mask : 0);
}

void CPU::SetZN(u8 val)
{
	SetStatusBit(STATUS_ZERO, val == 0);
	SetStatusBit(STATUS_NEGATIVE, val & 0x80);
}

void CPU::ExecuteOp()
{
	u8 result, tmp;
	u16 result16;
	constexpr u8 CONST = 0xFF;
	switch (m_CurInstr.op)
	{
	case Op::ADC:
		result16 = m_A + m_Val + (m_P & STATUS_CARRY ? 1 : 0);
		SetStatusBit(STATUS_CARRY, result16 > 0xFF);
		SetStatusBit(STATUS_ZERO, (result16 & 0xFF) == 0);
		SetStatusBit(STATUS_OVERFLOW, (result16 ^ m_A) & (result16 ^ m_Val) & 0x80);
		SetStatusBit(STATUS_NEGATIVE, result16 & 0x80);
		m_A = result16 & 0xFF;
		break;
	case Op::AND:
		m_A &= m_Val;
		SetZN(m_A);
		break;
	case Op::ASL:
		SetStatusBit(STATUS_CARRY, m_Val & 0x80);
		m_Val <<= 1;
		SetZN(m_Val);
		break;
	case Op::BCC:
		m_BranchTaken = !(m_P & STATUS_CARRY);
		break;
	case Op::BCS:
		m_BranchTaken = m_P & STATUS_CARRY;
		break;
	case Op::BEQ:
		m_BranchTaken = m_P & STATUS_ZERO;
		break;
	case Op::BIT:
		result = m_A & m_Val;
		SetStatusBit(STATUS_ZERO, result == 0);
		SetStatusBit(STATUS_OVERFLOW, m_Val & 0x40);
		SetStatusBit(STATUS_NEGATIVE, m_Val & 0x80);
		break;
	case Op::BMI:
		m_BranchTaken = m_P & STATUS_NEGATIVE;
		break;
	case Op::BNE:
		m_BranchTaken = !(m_P & STATUS_ZERO);
		break;
	case Op::BPL:
		m_BranchTaken = !(m_P & STATUS_NEGATIVE);
		break;
	case Op::BVC:
		m_BranchTaken = !(m_P & STATUS_OVERFLOW);
		break;
	case Op::BVS:
		m_BranchTaken = m_P & STATUS_OVERFLOW;
		break;
	case Op::CLC:
		m_P &= ~STATUS_CARRY;
		break;
	case Op::CLD:
		m_P &= ~STATUS_DECIMAL;
		break;
	case Op::CLI:
		m_P &= ~STATUS_INTERRUPT_DISABLE;
		break;
	case Op::CLV:
		m_P &= ~STATUS_OVERFLOW;
		break;
	case Op::CMP:
		SetStatusBit(STATUS_CARRY, m_A >= m_Val);
		SetStatusBit(STATUS_ZERO, m_A == m_Val);
		SetStatusBit(STATUS_NEGATIVE, (m_A - m_Val) & 0x80);
		break;
	case Op::CPX:
		SetStatusBit(STATUS_CARRY, m_X >= m_Val);
		SetStatusBit(STATUS_ZERO, m_X == m_Val);
		SetStatusBit(STATUS_NEGATIVE, (m_X - m_Val) & 0x80);
		break;
	case Op::CPY:
		SetStatusBit(STATUS_CARRY, m_Y >= m_Val);
		SetStatusBit(STATUS_ZERO, m_Y == m_Val);
		SetStatusBit(STATUS_NEGATIVE, (m_Y - m_Val) & 0x80);
		break;
	case Op::DEC:
		m_Val--;
		SetZN(m_Val);
		break;
	case Op::DEX:
		m_X--;
		SetZN(m_X);
		break;
	case Op::DEY:
		m_Y--;
		SetZN(m_Y);
		break;
	case Op::EOR:
		m_A ^= m_Val;
		SetZN(m_A);
		break;
	case Op::INC:
		m_Val++;
		SetZN(m_Val);
		break;
	case Op::INX:
		m_X++;
		SetZN(m_X);
		break;
	case Op::INY:
		m_Y++;
		SetZN(m_Y);
		break;
	case Op::LDA:
		m_A = m_Val;
		SetZN(m_A);
		break;
	case Op::LDX:
		m_X = m_Val;
		SetZN(m_X);
		break;
	case Op::LDY:
		m_Y = m_Val;
		SetZN(m_Y);
		break;
	case Op::LSR:
		SetStatusBit(STATUS_CARRY, m_Val & 0x1);
		m_Val >>= 1;
		SetStatusBit(STATUS_ZERO, m_Val == 0);
		m_P &= ~STATUS_NEGATIVE;
		break;
	case Op::ORA:
		m_A |= m_Val;
		SetZN(m_A);
		break;
	case Op::ROL:
		result = (m_Val << 1) | ((m_P & STATUS_CARRY) ? 1 : 0);
		SetStatusBit(STATUS_CARRY, m_Val & 0x80);
		SetZN(result);
		m_Val = result;
		break;
	case Op::ROR:
		result = (m_Val >> 1) | ((m_P & STATUS_CARRY) ? 0x80 : 0);
		SetStatusBit(STATUS_CARRY, m_Val & 0x1);
		SetZN(result);
		m_Val = result;
		break;
	case Op::SBC:
		tmp = ~m_Val;
		result16 = m_A + tmp + ((m_P & STATUS_CARRY) ? 1 : 0);
		SetStatusBit(STATUS_CARRY, result16 > 0xFF);
		SetStatusBit(STATUS_ZERO, (result16 & 0xFF) == 0);
		SetStatusBit(STATUS_OVERFLOW, (result16 ^ m_A) & (result16 ^ tmp) & 0x80);
		SetStatusBit(STATUS_NEGATIVE, result16 & 0x80);
		m_A = result16 & 0xFF;
		break;
	case Op::SEC:
		m_P |= STATUS_CARRY;
		break;
	case Op::SED:
		m_P |= STATUS_DECIMAL;
		break;
	case Op::SEI:
		m_P |= STATUS_INTERRUPT_DISABLE;
		break;
	case Op::STA:
		m_Val = m_A; 
		break;
	case Op::STX:
		m_Val = m_X;
		break;
	case Op::STY:
		m_Val = m_Y;
		break;
	case Op::TAX:
		m_X = m_A;
		SetZN(m_X);
		break;
	case Op::TAY:
		m_Y = m_A;
		SetZN(m_Y);
		break;
	case Op::TSX:
		m_X = m_S;
		SetZN(m_X);
		break;
	case Op::TXA:
		m_A = m_X;
		SetZN(m_A);
		break;
	case Op::TXS:
		m_S = m_X;
		break;
	case Op::TYA:
		m_A = m_Y;
		SetZN(m_A);
		break;
	case Op::NOP:
		break;
	case Op::ALR:
		m_A &= m_Val;
		SetStatusBit(STATUS_CARRY, m_A & 0x1);
		m_A >>= 1;
		SetStatusBit(STATUS_ZERO, m_A == 0);
		m_P &= ~STATUS_NEGATIVE;
		break;
	// Illegal ops
	case Op::ANC:
		m_A &= m_Val;
		SetStatusBit(STATUS_CARRY, m_A & 0x80);
		SetZN(m_A);
		break;
	case Op::ANE: 
		m_A = (m_A | CONST) & m_X & m_Val;
		SetZN(m_A);
		break;
	case Op::ARR:
		// no idea if this is correct
		m_A = ((m_A & m_Val) >> 1) | ((m_P & STATUS_CARRY) ? 0x80 : 0x0);
		SetStatusBit(STATUS_CARRY, m_A & 0x40);
		SetStatusBit(STATUS_OVERFLOW, ((m_A >> 6) ^ (m_A >> 5)) & 1); // wtf
		SetZN(m_A);
		break;
	case Op::DCP:
		m_Val--;
		SetStatusBit(STATUS_CARRY, m_A >= m_Val);
		SetStatusBit(STATUS_ZERO, m_A == m_Val);
		SetStatusBit(STATUS_NEGATIVE, (m_A - m_Val) & 0x80);
		break;
	case Op::ISC:
		m_Val++;
		tmp = ~m_Val;
		result16 = m_A + tmp + ((m_P & STATUS_CARRY) ? 1 : 0);
		SetStatusBit(STATUS_CARRY, result16 > 0xFF);
		SetStatusBit(STATUS_ZERO, (result16 & 0xFF) == 0);
		SetStatusBit(STATUS_OVERFLOW, (result16 ^ m_A) & (result16 ^ tmp) & 0x80);
		SetStatusBit(STATUS_NEGATIVE, result16 & 0x80);
		m_A = result16 & 0xFF;
		break;
	case Op::LAS:
		m_A = m_X = m_S = m_Val & m_S;
		SetZN(m_A);
		break;
	case Op::LAX:
		m_A = m_X = m_Val;
		SetZN(m_A);
		break;
	case Op::LXA:
		m_A = m_X = (m_A | CONST) & m_Val;
		SetZN(m_A);
		break;
	case Op::RLA:
		result = (m_Val << 1) | ((m_P & STATUS_CARRY) ? 1 : 0);
		m_A = m_A & result;
		SetStatusBit(STATUS_CARRY, m_Val & 0x80);
		SetZN(m_A);
		m_Val = result;
		break;
	case Op::RRA:
		result = (m_Val >> 1) | ((m_P & STATUS_CARRY) ? 0x80 : 0);
		result16 = m_A + result + ((m_Val & 1) ? 1 : 0);
		SetStatusBit(STATUS_CARRY, result16 > 0xFF);
		SetStatusBit(STATUS_ZERO, (result16 & 0xFF) == 0);
		SetStatusBit(STATUS_OVERFLOW, (result16 ^ m_A) & (result16 ^ result) & 0x80);
		SetStatusBit(STATUS_NEGATIVE, result16 & 0x80);
		m_A = result16 & 0xFF;
		m_Val = result;
		break;
	case Op::SAX:
		m_Val = m_A & m_X;
		//SetZN(m_Val);
		break;
	case Op::SBX:
		result = (m_A & m_X) + ~m_Val + 1;
		SetStatusBit(STATUS_CARRY, (m_A & m_X) >= m_Val); // not sure if carry flag is correct
		SetStatusBit(STATUS_ZERO, result == 0);
		SetStatusBit(STATUS_NEGATIVE, result & 0x80);
		m_X = result;
		break;
	case Op::SHA:
		m_Val = m_A & m_X & ((m_Addr >> 8) + 1);
		break;
	case Op::SHX:
		m_Val = m_X & ((m_Addr >> 8) + 1);
		break;
	case Op::SHY:
		m_Val = m_Y & ((m_Addr >> 8) + 1);
		break;
	case Op::SLO:
		SetStatusBit(STATUS_CARRY, m_Val & 0x80);
		m_Val <<= 1;
		m_A |= m_Val;
		SetZN(m_A);
		break;
	case Op::SRE:
		SetStatusBit(STATUS_CARRY, m_Val & 0x1);
		m_Val >>= 1;
		m_A ^= m_Val;
		SetZN(m_A);
		break;
	case Op::TAS:
		m_S = m_A & m_X;
		m_Val = m_A & m_X & ((m_Addr >> 8) + 1);
		break;
	default:
		ASSERT(false);
	}
}
