#pragma once

#include "../Core/Common.h"

enum class IndexType { X, Y };

class CPU;

enum class Op
{
	None, 
	ADC, AND, ASL, BCC, BCS, BEQ, BIT,
	BMI, BNE, BPL, BRK, BVC, BVS, CLC,
	CLD, CLI, CLV, CMP, CPX, CPY, DEC, 
	DEX, DEY, EOR, INC, INX, INY, JMP,
	JSR, LDA, LDX, LDY, LSR, NOP, ORA,
	PHA, PHP, PLA, PLP, ROL, ROR, RTI, 
	RTS, SBC, SEC, SED, SEI, STA, STX,
	STY, TAX, TAY, TSX, TXA, TXS, TYA
};

enum class InstrType
{
	None,
	Interrupt, 
	RTI, RTS, PHA, PHP, PLA, PLP, JSR,
	Accumulator,
	Implied,
	Immediate,
	AbsoluteJMP, 
	AbsoluteRead,
	AbsoluteReadModifyWrite, 
	AbsoluteWrite,
	ZeroPageRead,
	ZeroPageReadModifyWrite,
	ZeroPageWrite,
	ZeroPageXRead,
	ZeroPageYRead,
	ZeroPageXReadModifyWrite,
	ZeroPageXWrite,
	ZeroPageYWrite,
	AbsoluteXRead,
	AbsoluteYRead,
	AbsoluteXReadModifyWrite,
	AbsoluteXWrite,
	AbsoluteYWrite,
	Relative,
	IndexedIndirectRead,
	IndexedIndirectReadModifyWrite,
	IndexedIndirectWrite,
	IndirectIndexedRead,
	IndirectIndexedReadModifyWrite,
	IndirectIndexedWrite,
	AbsoluteIndirect
};

enum class InterruptType
{
	None, BRK, NMI, IRQ, RES
};

struct Instruction
{
	Op op = Op::None;
	InstrType type = InstrType::None;
};

class CPUBus;

class CPU
{
public:
	CPU();
	explicit CPU(CPUBus* bus);

	void Attach(CPUBus* bus) { m_Bus = bus; }

	void PerformCycle();

	void Reset();

	void SetNMILine(bool asserted);

	void SetIRQLine(bool asserted);

private:
	void PrintState();

	void StepInstruction();

	void PollInterrupts();

	u8 PopStack();
	void PushStack(u8 val);
	u8 ReadStack() const;

	u8 Read(u16 addr) const;
	void Write(u16 addr, u8 val) const;

	void SetStatusBit(u8 mask, bool cond);
	void SetZN(u8 val);

	void ExecuteOp();

	// ------- Cycle functions -----------

	void Interrupt();

	void RTI(); void RTS(); void PHA(); void PHP(); 
	void PLA(); void PLP(); void JSR();

	void Accumulator(); void Implied(); void Immediate();

	void AbsoluteJMP(); void AbsoluteRead();
	void AbsoluteReadModifyWrite(); void AbsoluteWrite();

	void ZeroPageRead(); void ZeroPageReadModifyWrite();
	void ZeroPageWrite(); 

	template <IndexType IT> void ZeroPageIndexedRead(); 
	void ZeroPageXReadModifyWrite();
	template <IndexType IT> void ZeroPageIndexedWrite();

	template <IndexType IT> void AbsoluteIndexedRead();
	void AbsoluteXReadModifyWrite();
	template <IndexType IT> void AbsoluteIndexedWrite();

	void Relative();

	void IndexedIndirectRead(); void IndexedIndirectReadModifyWrite();
	void IndexedIndirectWrite();

	void IndirectIndexedRead(); void IndirectIndexedReadModifyWrite();
	void IndirectIndexedWrite();

	void AbsoluteIndirect();

	// ---------------------------------------
	

private:
	static const Array<Instruction, 256> s_OpcodeLookup;

private:
	CPUBus* m_Bus = nullptr;

	Instruction m_CurInstr{};
	u8 m_InstrCycle = 0;

	// Registers
	u8 m_A = 0;
	u8 m_X = 0;
	u8 m_Y = 0;
	u16 m_PC = 0;
	u8 m_S = 0;
	u8 m_P = 0;

	u16 m_Addr = 0;
	u8 m_Val = 0;

	u8 m_Carry = 0;
	bool m_BranchTaken = false;
	bool m_InstrDone = false;

	bool m_NMILinePrev = false;
	bool m_NMILine = false;
	bool m_IRQLine = false;

	bool m_NMIPending = false;
	bool m_IRQPending = false;

	InterruptType m_CurInterrupt = InterruptType::None;

	Array<u8, 4> m_ReadBuf{};
	u64 m_TotalCycles = 0;
};
