#pragma once

#include "Common.h"
#include <array>

class Bus;

enum class StatusFlag : u8
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

typedef u8 (CPU::*InstructionFunc)(AddrMode);

struct Instruction
{
	InstrType instrType = InstrType::None;
	AddrMode addrMode = AddrMode::None;
	int byteCount = 0;
	int cycleCount = 0;
};

inline StatusFlag operator|(StatusFlag a, StatusFlag b)
{
	return static_cast<StatusFlag>(static_cast<u8>(a) | static_cast<u8>(b));
}

inline StatusFlag operator&(StatusFlag a, StatusFlag b)
{
	return static_cast<StatusFlag>(static_cast<u8>(a) & static_cast<u8>(b));
}

inline StatusFlag operator~(StatusFlag a)
{
	return static_cast<StatusFlag>(~static_cast<u8>(a));
}

class StatusRegister
{
public:
	StatusRegister() = default;
	explicit StatusRegister(u8 flags) : m_Flags{ flags } {}

	StatusRegister& Set(StatusFlag flag) { m_Flags |= static_cast<u8>(flag); return *this; }
	StatusRegister& Set(StatusFlag flag, bool on) { return on ? Set(flag) : Clear(flag); }

	StatusRegister& Clear(StatusFlag flag) { m_Flags &= ~static_cast<u8>(flag); return *this; }

	bool Test(StatusFlag flag) const { return m_Flags & static_cast<u8>(flag); }

	StatusRegister operator|(StatusFlag flag) const { return StatusRegister{ static_cast<u8>(m_Flags | static_cast<u8>(flag)) }; }
	StatusRegister operator&(StatusFlag flag) const { return StatusRegister{ static_cast<u8>(m_Flags & static_cast<u8>(flag)) }; }
	StatusRegister& operator|=(StatusFlag flag) { return Set(flag); }
	StatusRegister& operator&=(StatusFlag flag) { m_Flags &= static_cast<u8>(flag); return *this; }
	StatusRegister& operator=(StatusFlag flag) { m_Flags = static_cast<u8>(flag); return *this; }

	operator bool() const { return m_Flags != 0; }
	bool operator!() const { return m_Flags == 0; }

	u8 GetRaw() const { return m_Flags; }

	u8& GetRaw() { return m_Flags; }

private:
	u8 m_Flags = 0;
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

	u8 Read(u16 addr) const;
	u16 ReadWord(u16 addr) const;

	void Write(u16 addr, u8 val);
	void WriteWord(u16 addr, u16 val);

	void PushStack(u8 val);
	void PushStackWord(u16 val);

	u8 PopStack();
	u16 PopStackWord();

	u8 PeekStack() const;
	u16 PeekStackWord() const;

	bool AddsCycle(AddrMode addrMode) const;

	u8 Branch(u16 addr);

	// ========== Address modes =========

	u16 ResolveAddress(AddrMode) const;

	u16 ResolveImplicit() const;
	u16 ResolveAccumulator() const;
	u16 ResolveImmediate() const;
	u16 ResolveZeroPage() const;
	u16 ResolveZeroPageX() const;
	u16 ResolveZeroPageY() const;
	u16 ResolveRelative() const;
	u16 ResolveAbsolute() const;
	u16 ResolveAbsoluteX() const;
	u16 ResolveAbsoluteY() const;
	u16 ResolveIndirect() const;
	u16 ResolveIndexedIndirect() const;
	u16 ResolveIndirectIndexed() const;

	// ========== Operations ============

	u8 ADC(AddrMode); u8 AND(AddrMode); u8 ASL(AddrMode); u8 BCC(AddrMode);
	u8 BCS(AddrMode); u8 BEQ(AddrMode); u8 BIT(AddrMode); u8 BMI(AddrMode);
	u8 BNE(AddrMode); u8 BPL(AddrMode); u8 BRK(AddrMode); u8 BVC(AddrMode);
	u8 BVS(AddrMode); u8 CLC(AddrMode); u8 CLD(AddrMode); u8 CLI(AddrMode);
	u8 CLV(AddrMode); u8 CMP(AddrMode); u8 CPX(AddrMode); u8 CPY(AddrMode);
	u8 DEC(AddrMode); u8 DEX(AddrMode); u8 DEY(AddrMode); u8 EOR(AddrMode);
	u8 INC(AddrMode); u8 INX(AddrMode); u8 INY(AddrMode); u8 JMP(AddrMode);
	u8 JSR(AddrMode); u8 LDA(AddrMode); u8 LDX(AddrMode); u8 LDY(AddrMode);
	u8 LSR(AddrMode); u8 NOP(AddrMode); u8 ORA(AddrMode); u8 PHA(AddrMode);
	u8 PHP(AddrMode); u8 PLA(AddrMode); u8 PLP(AddrMode); u8 ROL(AddrMode);
	u8 ROR(AddrMode); u8 RTI(AddrMode); u8 RTS(AddrMode); u8 SBC(AddrMode);
	u8 SEC(AddrMode); u8 SED(AddrMode); u8 SEI(AddrMode); u8 STA(AddrMode);
	u8 STX(AddrMode); u8 STY(AddrMode); u8 TAX(AddrMode); u8 TAY(AddrMode);
	u8 TSX(AddrMode); u8 TXA(AddrMode); u8 TXS(AddrMode); u8 TYA(AddrMode);

private:
	Bus* m_Bus = nullptr;

	StatusRegister m_StatusReg{};
	
	struct
	{
		u8 A;
		u8 X;
		u8 Y;
		u16 PC;
		u8 S;
	} m_Regs{};

	Instruction m_CurrentInstruction{};

	u8 m_InstructionRemainingCycles = 0;
	u8 m_InterruptDisableDelay = 0;

	bool m_PCManuallySet = false;

	u64 m_TotalCycles = 0;
};