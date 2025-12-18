#include "VirtualController.h"
#include "../Core/Logger.h"

void VirtualController::SetButtonState(ControllerButton button, bool pressed)
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

u8 VirtualController::ReadBit()
{
	const u8 ret = m_ShiftReg & 1;
	if (!m_ShiftStrobe)
	{
		// Fill upper bits with 1
		m_ShiftReg = (1 << 7) | (m_ShiftReg >> 1);
	}

	return ret;
}

void VirtualController::Write(bool bit)
{
	m_ShiftStrobe = bit;
	if (m_ShiftStrobe)
	{
		m_ShiftReg = m_ControllerState;
	}
}