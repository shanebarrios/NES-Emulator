#pragma once

#include "../Core/Common.h"

enum class ControllerButton
{
	A,
	B,
	Select,
	Start,
	Up,
	Down,
	Left,
	Right
};

class VirtualController
{
public:
	void SetButtonState(ControllerButton button, bool pressed);

	u8 ReadBit();

	void Write(bool bit);

private:
	bool m_ShiftStrobe = 0;
	u8 m_ControllerState = 0;
	u8 m_ShiftReg = 0;
};