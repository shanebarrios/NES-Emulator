#include "DebugUtils.h"

#include "CPU.h" 
#include <cstring>

namespace DebugUtils
{
	std::array<char, 2> ByteToChars(u8 byte)
	{
		static constexpr char chars[17] = "0123456789ABCDEF";
		const char c1 = chars[byte >> 4];
		const char c2 = chars[byte & 0xF];
		return { c1, c2 };
	}

	const char* OpToStr(Op op)
	{
		switch (op)
		{
		case Op::None: return nullptr;
		case Op::ADC:  return "ADC";
		case Op::AND:  return "AND";
		case Op::ASL:  return "ASL";
		case Op::BCC:  return "BCC";
		case Op::BCS:  return "BCS";
		case Op::BEQ:  return "BEQ";
		case Op::BIT:  return "BIT";
		case Op::BMI:  return "BMI";
		case Op::BNE:  return "BNE";
		case Op::BPL:  return "BPL";
		case Op::BRK:  return "BRK";
		case Op::BVC:  return "BVC";
		case Op::BVS:  return "BVS";
		case Op::CLC:  return "CLC";
		case Op::CLD:  return "CLD";
		case Op::CLI:  return "CLI";
		case Op::CLV:  return "CLV";
		case Op::CMP:  return "CMP";
		case Op::CPX:  return "CPX";
		case Op::CPY:  return "CPY";
		case Op::DEC:  return "DEC";
		case Op::DEX:  return "DEX";
		case Op::DEY:  return "DEY";
		case Op::EOR:  return "EOR";
		case Op::INC:  return "INC";
		case Op::INX:  return "INX";
		case Op::INY:  return "INY";
		case Op::JMP:  return "JMP";
		case Op::JSR:  return "JSR";
		case Op::LDA:  return "LDA";
		case Op::LDX:  return "LDX";
		case Op::LDY:  return "LDY";
		case Op::LSR:  return "LSR";
		case Op::NOP:  return "NOP";
		case Op::ORA:  return "ORA";
		case Op::PHA:  return "PHA";
		case Op::PHP:  return "PHP";
		case Op::PLA:  return "PLA";
		case Op::PLP:  return "PLP";
		case Op::ROL:  return "ROL";
		case Op::ROR:  return "ROR";
		case Op::RTI:  return "RTI";
		case Op::RTS:  return "RTS";
		case Op::SBC:  return "SBC";
		case Op::SEC:  return "SEC";
		case Op::SED:  return "SED";
		case Op::SEI:  return "SEI";
		case Op::STA:  return "STA";
		case Op::STX:  return "STX";
		case Op::STY:  return "STY";
		case Op::TAX:  return "TAX";
		case Op::TAY:  return "TAY";
		case Op::TSX:  return "TSX";
		case Op::TXA:  return "TXA";
		case Op::TXS:  return "TXS";
		case Op::TYA:  return "TYA";
		default: return nullptr;
		}
	}

	/*void AddrModeToStr(AddrMode mode, u16 operand, u16 effectiveAddr, u8 val, char* buf, usize bufLen)
	{
		switch (mode)
		{
		case AddrMode::None: 
		case AddrMode::Implicit: buf[0] = '\0'; return;
		case AddrMode::Accumulator:
			std::snprintf(buf, bufLen, "A"); return;
		case AddrMode::Immediate:
			std::snprintf(buf, bufLen, "#$%02X", val); return;
		case AddrMode::ZeroPage:
			std::snprintf(buf, bufLen, "$%02X = %02X", effectiveAddr, val); return;
		case AddrMode::ZeroPageX:
			std::snprintf(buf, bufLen, "$%02X,X @ %02X = %02X", operand, effectiveAddr, val); return;
		case AddrMode::ZeroPageY:
			std::snprintf(buf, bufLen, "$%02X,Y @ %02X = %02X", operand, effectiveAddr, val); return;
		case AddrMode::Relative:
			std::snprintf(buf, bufLen, "$%04X", effectiveAddr); return;
		case AddrMode::Absolute:
			std::snprintf(buf, bufLen, "$%04X", effectiveAddr); return;
		case AddrMode::AbsoluteX:
			std::snprintf(buf, bufLen, "$%04X,X @ %04X = %02X", operand, effectiveAddr, val); return;
		case AddrMode::AbsoluteY:
			std::snprintf(buf, bufLen, "$%04X,Y @ %04X = %02X", operand, effectiveAddr, val); return;
		case AddrMode::Indirect:
			std::snprintf(buf, bufLen, "($%04X) = %04X", operand, effectiveAddr); return;
		case AddrMode::IndexedIndirect:
		case AddrMode::IndirectIndexed:
		default:
			buf[0] = '\0'; return;
		}
	}*/
}