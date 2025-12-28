#include "Input.h"

#include "WindowsCommon.h"
#include "Window.h"
#include "Logger.h"
#include <Xinput.h>
#include <cmath>
#include <numbers>

struct ControllerState
{
	u32 buttons;
	bool connected;
};

static constexpr usize MAX_CONTROLLERS = XUSER_MAX_COUNT;

static constexpr std::array XINPUT_BUTTONS =
{
	XINPUT_GAMEPAD_DPAD_UP, XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_RIGHT,
	XINPUT_GAMEPAD_START, XINPUT_GAMEPAD_BACK,
	XINPUT_GAMEPAD_LEFT_THUMB, XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER,
	XINPUT_GAMEPAD_A, XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_X, XINPUT_GAMEPAD_Y
};

static Array<ControllerState, MAX_CONTROLLERS> s_ControllersState = {};
static const Window* s_FocusWindow = nullptr;

// ngl chatgpt wrote this for me (rare case where AI is actually useful)
static GamepadButton XInputToAbstract(u32 xInputButton)
{
	switch (xInputButton)
	{
	case XINPUT_GAMEPAD_DPAD_UP:        return GamepadButton::DPadUp;
	case XINPUT_GAMEPAD_DPAD_DOWN:      return GamepadButton::DPadDown;
	case XINPUT_GAMEPAD_DPAD_LEFT:      return GamepadButton::DPadLeft;
	case XINPUT_GAMEPAD_DPAD_RIGHT:     return GamepadButton::DPadRight;
	case XINPUT_GAMEPAD_START:          return GamepadButton::Start;
	case XINPUT_GAMEPAD_BACK:           return GamepadButton::Back;
	case XINPUT_GAMEPAD_LEFT_THUMB:     return GamepadButton::LeftThumb;
	case XINPUT_GAMEPAD_RIGHT_THUMB:    return GamepadButton::RightThumb;
	case XINPUT_GAMEPAD_LEFT_SHOULDER:  return GamepadButton::LeftShoulder;
	case XINPUT_GAMEPAD_RIGHT_SHOULDER: return GamepadButton::RightShoulder;
	case XINPUT_GAMEPAD_A:              return GamepadButton::A;
	case XINPUT_GAMEPAD_B:              return GamepadButton::B;
	case XINPUT_GAMEPAD_X:              return GamepadButton::X;
	case XINPUT_GAMEPAD_Y:              return GamepadButton::Y;
	default:                            ASSERT(false); return GamepadButton::Null;
	}
}

struct LeftStickDirections
{
	static constexpr GamepadButton Up = GamepadButton::LeftStickUp;
	static constexpr GamepadButton Down = GamepadButton::LeftStickDown;
	static constexpr GamepadButton Left = GamepadButton::LeftStickLeft;
	static constexpr GamepadButton Right = GamepadButton::LeftStickRight;
};

struct RightStickDirections
{
	static constexpr GamepadButton Up = GamepadButton::RightStickUp;
	static constexpr GamepadButton Down = GamepadButton::RightStickDown;
	static constexpr GamepadButton Left = GamepadButton::RightStickLeft;
	static constexpr GamepadButton Right = GamepadButton::RightStickRight;
};

template <typename Direction, typename T>
requires std::is_arithmetic_v<T>
static GamepadButton JoystickInputToButton(T x, T y)
{
	const auto theta = std::atan2(y, x);
	constexpr double PI = std::numbers::pi;

	if (theta > -PI / 4.0 && theta <= PI / 4.0)
	{
		return Direction::Right;
	}
	else if (theta > PI / 4.0 && theta <= 3.0 * PI / 4.0)
	{
		return Direction::Up;
	}
	else if (theta > 3 * PI / 4.0 || theta <= -3 * PI / 4.0)
	{
		return Direction::Left;
	}
	else
	{
		return Direction::Down;
	}
}

static void Win32PollControllerInput()
{
	DWORD dwResult;
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		dwResult = XInputGetState(i, &state);

		if (dwResult != ERROR_SUCCESS)
		{
			s_ControllersState[i].buttons = 0;
			s_ControllersState[i].connected = false;
			continue;
		}

		const XINPUT_GAMEPAD& gamepad = state.Gamepad;
		const WORD wButtons = state.Gamepad.wButtons;
		u32 buttons = 0;

		for (auto mask : XINPUT_BUTTONS)
		{
			const GamepadButton translated = XInputToAbstract(mask);
			if (wButtons & mask)
			{
				buttons |= static_cast<u32>(translated);
			}
		}

		if (gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			buttons |= static_cast<u32>(GamepadButton::LeftTrigger);
		}
		if (gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			buttons |= static_cast<u32>(GamepadButton::RightTrigger);
		}

		const int leftX = gamepad.sThumbLX;
		const int leftY = gamepad.sThumbLY;
		const double leftMagnitude = std::sqrt(leftX * leftX + leftY * leftY);
		if (leftMagnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			const GamepadButton button = JoystickInputToButton<LeftStickDirections>(leftX, leftY);

			buttons |= static_cast<u32>(button);
		}

		const SHORT rightX = gamepad.sThumbRX;
		const SHORT rightY = gamepad.sThumbRY;
		const double rightMagnitude = std::sqrt(rightX * rightX + rightY * rightY);
		if (static_cast<SHORT>(rightMagnitude) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			const GamepadButton button = JoystickInputToButton<RightStickDirections>(rightX, rightY);

			buttons |= static_cast<u32>(button);
		}

		s_ControllersState[i].buttons = buttons;
		s_ControllersState[i].connected = true;
	}
}

static void Win32PollEvents()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	Win32PollControllerInput();
}

namespace Input
{
	void SetFocus(const Window& window)
	{
		s_FocusWindow = &window;
	}

	void PollEvents()
	{
		Win32PollEvents();
	}

	bool IsPressed(KeyCode keyCode)
	{
		ASSERT(s_FocusWindow);
		
		return s_FocusWindow->IsKeyDown(keyCode);
	}

	bool IsPressed(GamepadButton button, int controllerIndex)
	{
		if (controllerIndex < 0 || controllerIndex > 3)
		{
			return false;
		}

		const u32 buttons = s_ControllersState[controllerIndex].buttons;
		return buttons & static_cast<u32>(button);
	}

	bool ControllerConnected(int index)
	{
		if (index < 0 || index > MAX_CONTROLLERS) return false;

		return s_ControllersState[index].connected;
	}
}

