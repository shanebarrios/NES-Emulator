#pragma once

#include "../Core/Common.h"

enum class NESButton
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

class HardwareController
{
public:
	void SetButtonsState(u8 state);

	void SetButtonState(NESButton button, bool pressed);

	u8 ReadBit();

	void Write(bool bit);

private:
	bool m_ShiftStrobe = 0;
	u8 m_ControllerState = 0;
	u8 m_ShiftReg = 0;
};