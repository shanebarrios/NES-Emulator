#include "DebugUtils.h"

#include "CPU.h"

namespace DebugUtils
{
	std::array<char, 2> ByteToChars(uint8_t byte)
	{
		static constexpr char chars[17] = "0123456789ABCDEF";
		const char c1 = chars[byte >> 4];
		const char c2 = chars[byte & 0xF];
		return { c1, c2 };
	}

	const char* InstrTypeToStr(InstrType instrType)
	{
		switch (instrType)
		{
		case InstrType::None: return nullptr;
		case InstrType::ADC:  return "ADC";
		case InstrType::AND:  return "AND";
		case InstrType::ASL:  return "ASL";
		case InstrType::BCC:  return "BCC";
		case InstrType::BCS:  return "BCS";
		case InstrType::BEQ:  return "BEQ";
		case InstrType::BIT:  return "BIT";
		case InstrType::BMI:  return "BMI";
		case InstrType::BNE:  return "BNE";
		case InstrType::BPL:  return "BPL";
		case InstrType::BRK:  return "BRK";
		case InstrType::BVC:  return "BVC";
		case InstrType::BVS:  return "BVS";
		case InstrType::CLC:  return "CLC";
		case InstrType::CLD:  return "CLD";
		case InstrType::CLI:  return "CLI";
		case InstrType::CLV:  return "CLV";
		case InstrType::CMP:  return "CMP";
		case InstrType::CPX:  return "CPX";
		case InstrType::CPY:  return "CPY";
		case InstrType::DEC:  return "DEC";
		case InstrType::DEX:  return "DEX";
		case InstrType::DEY:  return "DEY";
		case InstrType::EOR:  return "EOR";
		case InstrType::INC:  return "INC";
		case InstrType::INX:  return "INX";
		case InstrType::INY:  return "INY";
		case InstrType::JMP:  return "JMP";
		case InstrType::JSR:  return "JSR";
		case InstrType::LDA:  return "LDA";
		case InstrType::LDX:  return "LDX";
		case InstrType::LDY:  return "LDY";
		case InstrType::LSR:  return "LSR";
		case InstrType::NOP:  return "NOP";
		case InstrType::ORA:  return "ORA";
		case InstrType::PHA:  return "PHA";
		case InstrType::PHP:  return "PHP";
		case InstrType::PLA:  return "PLA";
		case InstrType::PLP:  return "PLP";
		case InstrType::ROL:  return "ROL";
		case InstrType::ROR:  return "ROR";
		case InstrType::RTI:  return "RTI";
		case InstrType::RTS:  return "RTS";
		case InstrType::SBC:  return "SBC";
		case InstrType::SEC:  return "SEC";
		case InstrType::SED:  return "SED";
		case InstrType::SEI:  return "SEI";
		case InstrType::STA:  return "STA";
		case InstrType::STX:  return "STX";
		case InstrType::STY:  return "STY";
		case InstrType::TAX:  return "TAX";
		case InstrType::TAY:  return "TAY";
		case InstrType::TSX:  return "TSX";
		case InstrType::TXA:  return "TXA";
		case InstrType::TXS:  return "TXS";
		case InstrType::TYA:  return "TYA";
		default: return nullptr;
		}
	}
}