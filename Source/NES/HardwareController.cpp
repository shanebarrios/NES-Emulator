#include "HardwareController.h"
#include "../Core/Logger.h"

void HardwareController::SetButtonState(NESButton button, bool pressed)
{
	if (pressed)
	{
		m_ControllerState |= (1 << static_cast<u8>(button));
	}
	else
	{
		m_ControllerState &= ~(1 << static_cast<u8>(button));
	}
	if (m_ShiftStrobe)
	{
		m_ShiftReg = m_ControllerState;
	}
}

void HardwareController::SetButtonsState(u8 state)
{
	m_ControllerState = state;
	if (m_ShiftStrobe)
	{
		m_ShiftReg = m_ControllerState;
	}
}

u8 HardwareController::ReadBit()
{
	const u8 ret = m_ShiftReg & 1;
	if (!m_ShiftStrobe)
	{
		// Fill upper bits with 1
		m_ShiftReg = (1 << 7) | (m_ShiftReg >> 1);
	}

	return ret;
}

void HardwareController::Write(bool bit)
{
	m_ShiftStrobe = bit;
	if (m_ShiftStrobe)
	{
		m_ShiftReg = m_ControllerState;
	}
}