#pragma once

#include <cstdint>
#include <array>
#include <optional>

class Bus;

enum class StatusFlag : uint8_t
{
	Carry = 1 << 0,
	Zero = 1 << 1,
	InterruptDisable = 1 << 2,
	Decimal = 1 << 3,
	Break = 1 << 4,
	Unused = 1 << 5,
	Overflow = 1 << 6,
	Negative = 1 << 7
};

enum class AddrMode
{
	None,
	Implicit,
	Accumulator,
	Immediate,
	ZeroPage,
	ZeroPageX,
	ZeroPageY,
	Relative,
	Absolute,
	AbsoluteX,
	AbsoluteY,
	Indirect,
	IndexedIndirect,
	IndirectIndexed
};

enum class InstrType
{
	None,
	ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC,
	CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP, 
	JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, ROR, RTI,
	RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA
};

class CPU;

typedef uint8_t (CPU::*InstructionFunc)(AddrMode);


struct Instruction
{
	InstrType instrType = InstrType::None;
	AddrMode addrMode = AddrMode::None;
	int byteCount = 0;
	int cycleCount = 0;
};

inline StatusFlag operator|(StatusFlag a, StatusFlag b)
{
	return static_cast<StatusFlag>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline StatusFlag operator&(StatusFlag a, StatusFlag b)
{
	return static_cast<StatusFlag>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

inline StatusFlag operator~(StatusFlag a)
{
	return static_cast<StatusFlag>(~static_cast<uint8_t>(a));
}

class StatusRegister
{
public:
	StatusRegister() = default;
	explicit StatusRegister(uint8_t flags) : m_Flags{ flags } {}

	StatusRegister& Set(StatusFlag flag) { m_Flags |= static_cast<uint8_t>(flag); return *this; }
	StatusRegister& Set(StatusFlag flag, bool on) { return on ? Set(flag) : Clear(flag); }

	StatusRegister& Clear(StatusFlag flag) { m_Flags &= ~static_cast<uint8_t>(flag); return *this; }

	bool Test(StatusFlag flag) const { return m_Flags & static_cast<uint8_t>(flag); }

	StatusRegister operator|(StatusFlag flag) const { return StatusRegister{ static_cast<uint8_t>(m_Flags | static_cast<uint8_t>(flag)) }; }
	StatusRegister operator&(StatusFlag flag) const { return StatusRegister{ static_cast<uint8_t>(m_Flags & static_cast<uint8_t>(flag)) }; }
	StatusRegister& operator|=(StatusFlag flag) { return Set(flag); }
	StatusRegister& operator&=(StatusFlag flag) { m_Flags &= static_cast<uint8_t>(flag); return *this; }
	StatusRegister& operator=(StatusFlag flag) { m_Flags = static_cast<uint8_t>(flag); return *this; }

	operator bool() const { return m_Flags != 0; }
	bool operator!() const { return m_Flags == 0; }

	uint8_t GetRaw() const { return m_Flags; }

	uint8_t& GetRaw() { return m_Flags; }

private:
	uint8_t m_Flags = 0;
};

class CPU
{
public:
	CPU() = default;

	void Init(Bus* bus);

	void Reset();

	void PerformCycle();

	void PrintState() const;

private:
	static const std::array<Instruction, 256> s_OpcodeLookup;

private:
	void ExecuteInstruction();

	InstructionFunc ResolveInstructionFunction(InstrType instrType) const;

	uint8_t Read(uint16_t addr) const;
	uint16_t ReadWord(uint16_t addr) const;

	void Write(uint16_t addr, uint8_t val);
	void WriteWord(uint16_t addr, uint16_t val);

	void PushStack(uint8_t val);
	void PushStackWord(uint16_t val);

	uint8_t PopStack();
	uint16_t PopStackWord();

	uint8_t PeekStack() const;
	uint16_t PeekStackWord() const;

	bool AddsCycle(AddrMode addrMode) const;

	uint8_t Branch(uint16_t addr);

	// ========== Address modes =========

	uint16_t ResolveAddress(AddrMode) const;

	uint16_t ResolveImplicit() const;
	uint16_t ResolveAccumulator() const;
	uint16_t ResolveImmediate() const;
	uint16_t ResolveZeroPage() const;
	uint16_t ResolveZeroPageX() const;
	uint16_t ResolveZeroPageY() const;
	uint16_t ResolveRelative() const;
	uint16_t ResolveAbsolute() const;
	uint16_t ResolveAbsoluteX() const;
	uint16_t ResolveAbsoluteY() const;
	uint16_t ResolveIndirect() const;
	uint16_t ResolveIndexedIndirect() const;
	uint16_t ResolveIndirectIndexed() const;

	// ========== Operations ============

	uint8_t ADC(AddrMode); uint8_t AND(AddrMode); uint8_t ASL(AddrMode); uint8_t BCC(AddrMode);
	uint8_t BCS(AddrMode); uint8_t BEQ(AddrMode); uint8_t BIT(AddrMode); uint8_t BMI(AddrMode);
	uint8_t BNE(AddrMode); uint8_t BPL(AddrMode); uint8_t BRK(AddrMode); uint8_t BVC(AddrMode);
	uint8_t BVS(AddrMode); uint8_t CLC(AddrMode); uint8_t CLD(AddrMode); uint8_t CLI(AddrMode);
	uint8_t CLV(AddrMode); uint8_t CMP(AddrMode); uint8_t CPX(AddrMode); uint8_t CPY(AddrMode);
	uint8_t DEC(AddrMode); uint8_t DEX(AddrMode); uint8_t DEY(AddrMode); uint8_t EOR(AddrMode);
	uint8_t INC(AddrMode); uint8_t INX(AddrMode); uint8_t INY(AddrMode); uint8_t JMP(AddrMode);
	uint8_t JSR(AddrMode); uint8_t LDA(AddrMode); uint8_t LDX(AddrMode); uint8_t LDY(AddrMode);
	uint8_t LSR(AddrMode); uint8_t NOP(AddrMode); uint8_t ORA(AddrMode); uint8_t PHA(AddrMode);
	uint8_t PHP(AddrMode); uint8_t PLA(AddrMode); uint8_t PLP(AddrMode); uint8_t ROL(AddrMode);
	uint8_t ROR(AddrMode); uint8_t RTI(AddrMode); uint8_t RTS(AddrMode); uint8_t SBC(AddrMode);
	uint8_t SEC(AddrMode); uint8_t SED(AddrMode); uint8_t SEI(AddrMode); uint8_t STA(AddrMode);
	uint8_t STX(AddrMode); uint8_t STY(AddrMode); uint8_t TAX(AddrMode); uint8_t TAY(AddrMode);
	uint8_t TSX(AddrMode); uint8_t TXA(AddrMode); uint8_t TXS(AddrMode); uint8_t TYA(AddrMode);

private:
	Bus* m_Bus = nullptr;

	StatusRegister m_StatusReg{};
	
	struct
	{
		uint8_t A;
		uint8_t X;
		uint8_t Y;
		uint16_t PC;
		uint8_t S;
	} m_Regs{};

	Instruction m_CurrentInstruction{};

	uint8_t m_InstructionRemainingCycles = 0;
	uint8_t m_InterruptDisableDelay = 0;

	bool m_PCManuallySet = false;

	uint64_t m_TotalCycles = 0;
};