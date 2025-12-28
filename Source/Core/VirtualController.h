#pragma once

#include <variant>
#include "Common.h"
#include "Input.h"

struct KeyboardBinding
{
	KeyCode keyCode;
};

struct GamepadBinding
{
	int controllerIndex;
	GamepadButton button;

};

using InputBinding = std::variant<
	KeyboardBinding,
	GamepadBinding>;

enum class NESButton;

class VirtualController
{
public:
	VirtualController();

	bool IsPressed(NESButton nesButton) const;

	void SetBinding(NESButton nesButton, KeyCode keyCode);
	void SetBinding(NESButton nesButton, GamepadButton button, int controllerIndex = 0);

	void SetDefaultKeyboardBindings();
	void SetDefaultControllerBindings();

	u8 ToHardwareState() const;

private:
	Array<InputBinding, 8> m_Bindings{};
};