#include "VirtualController.h"
#include "../NES/HardwareController.h"

VirtualController::VirtualController()
{
	SetDefaultKeyboardBindings();
}

bool VirtualController::IsPressed(NESButton button) const
{
	const InputBinding binding = m_Bindings[static_cast<usize>(button)];

	return std::visit([](auto&& binding) -> bool {
		using T = std::decay_t<decltype(binding)>;
		if constexpr (std::is_same_v<T, KeyboardBinding>)
		{
			return Input::IsPressed(binding.keyCode);
		}
		else if constexpr (std::is_same_v<T, GamepadBinding>)
		{
			return Input::IsPressed(binding.button, binding.controllerIndex);
		}
		return false;
		}, binding);
}

void VirtualController::SetBinding(NESButton nesButton, KeyCode keyCode)
{
	m_Bindings[static_cast<usize>(nesButton)] =
		KeyboardBinding{ .keyCode = keyCode };
}

void VirtualController::SetBinding(NESButton nesButton, GamepadButton button, int controllerIndex)
{
	m_Bindings[static_cast<usize>(nesButton)] =
		GamepadBinding{ .controllerIndex = controllerIndex, .button = button };
}

void VirtualController::SetDefaultKeyboardBindings()
{
	SetBinding(NESButton::A, KeyCode::X);
	SetBinding(NESButton::B, KeyCode::Z);
	SetBinding(NESButton::Select, KeyCode::Q);
	SetBinding(NESButton::Start, KeyCode::W);
	SetBinding(NESButton::Up, KeyCode::UpArrow);
	SetBinding(NESButton::Down, KeyCode::DownArrow);
	SetBinding(NESButton::Left, KeyCode::LeftArrow);
	SetBinding(NESButton::Right, KeyCode::RightArrow);
}

void VirtualController::SetDefaultControllerBindings()
{
	// todo
	SetBinding(NESButton::A, GamepadButton::A);
	SetBinding(NESButton::B, GamepadButton::B);
	SetBinding(NESButton::Select, GamepadButton::Back);
	SetBinding(NESButton::Start, GamepadButton::Start);
	SetBinding(NESButton::Up, GamepadButton::LeftStickUp);
	SetBinding(NESButton::Down, GamepadButton::LeftStickDown);
	SetBinding(NESButton::Left, GamepadButton::LeftStickLeft);
	SetBinding(NESButton::Right, GamepadButton::LeftStickRight);
}

u8 VirtualController::ToHardwareState() const
{
	u8 buttons = 0;

	for (u8 i = 0; i < 8; i++)
	{
		const bool pressed = IsPressed(static_cast<NESButton>(i));
		if (pressed)
		{
			buttons |= 1 << i;
		}
	}

	return buttons;
}