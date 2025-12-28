#include "Input.h"

#include "WindowsCommon.h"
#include "Window.h"
#include "Logger.h"
#include <stdexcept>
#include <bitset>
#include <vector>
#include <GameInput.h>

using namespace GameInput::v3;

static constexpr usize MAX_CONTROLLERS = 4;

static IGameInput* s_GameInput = nullptr;
static Array<IGameInputDevice*, MAX_CONTROLLERS> s_Gamepads{};
static GameInputCallbackToken s_CallbackToken = 0;

static Array<GameInputGamepadButtons, MAX_CONTROLLERS> s_ControllersState {};
static std::bitset<256> s_KeyboardState{};
static u64 s_MouseX = 0;
static u64 s_MouseY = 0;


static GameInputGamepadButtons AbstractToGameInput(GamepadButton button)
{
	switch (button)
	{
	case GamepadButton::Null:            return GameInputGamepadNone;
	case GamepadButton::DPadDown:        return GameInputGamepadDPadDown;
	case GamepadButton::DPadLeft:        return GameInputGamepadDPadLeft;
	case GamepadButton::DPadRight:       return GameInputGamepadDPadRight;
	case GamepadButton::Start:           return GameInputGamepadMenu;
	case GamepadButton::Back:            return GameInputGamepadView;
	case GamepadButton::LeftThumb:       return GameInputGamepadLeftThumbstick;
	case GamepadButton::RightThumb:      return GameInputGamepadRightThumbstick;
	case GamepadButton::LeftShoulder:    return GameInputGamepadLeftShoulder;
	case GamepadButton::RightShoulder:   return GameInputGamepadRightShoulder;
	case GamepadButton::A:               return GameInputGamepadA;
	case GamepadButton::B:               return GameInputGamepadB;
	case GamepadButton::X:               return GameInputGamepadX;
	case GamepadButton::Y:               return GameInputGamepadY;
	case GamepadButton::LeftStickUp:     return GameInputGamepadLeftThumbstickUp;
	case GamepadButton::LeftStickDown:   return GameInputGamepadLeftThumbstickDown;
	case GamepadButton::LeftStickLeft:   return GameInputGamepadLeftThumbstickLeft;
	case GamepadButton::LeftStickRight:  return GameInputGamepadLeftThumbstickRight;
	case GamepadButton::RightStickUp:    return GameInputGamepadRightThumbstickUp;
	case GamepadButton::RightStickDown:  return GameInputGamepadRightThumbstickDown;
	case GamepadButton::RightStickLeft:  return GameInputGamepadRightThumbstickLeft;
	case GamepadButton::RightStickRight: return GameInputGamepadRightThumbstickRight;
	default:                             ASSERT(false); return GameInputGamepadNone;
	}
}

static void Win32PollInput()
{
	for (usize i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (!s_Gamepads[i]) continue;

		IGameInputReading* reading;
		if (FAILED(s_GameInput->GetCurrentReading(
			GameInputKindGamepad, 
			s_Gamepads[i], 
			&reading)))
		{
			continue;
		}

		GameInputGamepadState state;
		reading->GetGamepadState(&state);
		reading->Release();
		
		s_ControllersState[i] = state.buttons;
	}


	IGameInputReading* reading;
	s_KeyboardState.reset();
	if (SUCCEEDED(s_GameInput->GetCurrentReading(
		GameInputKindKeyboard | GameInputKindMouse,
		nullptr,
		&reading)))
	{
		const u32 count = reading->GetKeyCount();
		std::vector<GameInputKeyState> keys(count);
		const u32 returned = reading->GetKeyState(count, keys.data());
		for (usize i = 0; i < returned; i++)
		{
			const GameInputKeyState& state = keys[i];
			s_KeyboardState.set(state.virtualKey);
		}

		GameInputMouseState mouseState;
		if (reading->GetMouseState(&mouseState))
		{
			s_MouseX = mouseState.absolutePositionX;
			s_MouseY = mouseState.absolutePositionY;
			s_KeyboardState.set(
				static_cast<usize>(KeyCode::LeftMouse),
				mouseState.buttons & GameInputMouseLeftButton);
			s_KeyboardState.set(
				static_cast<usize>(KeyCode::RightMouse),
				mouseState.buttons & GameInputMouseRightButton);
			s_KeyboardState.set(
				static_cast<usize>(KeyCode::MiddleMouse),
				mouseState.buttons & GameInputMouseMiddleButton);
			s_KeyboardState.set(
				static_cast<usize>(KeyCode::Mouse4),
				mouseState.buttons & GameInputMouseButton4);
			s_KeyboardState.set(
				static_cast<usize>(KeyCode::Mouse5),
				mouseState.buttons & GameInputMouseButton5);
		}
		reading->Release();
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
	Win32PollInput();
}

static void DeviceStatusCallback(
	GameInputCallbackToken callbackToken,
	void* context,
	IGameInputDevice* device,
	uint64_t timestamp,
	GameInputDeviceStatus currentStatus,
	GameInputDeviceStatus previousStatus)
{
	// connected
	if ((currentStatus & GameInputDeviceConnected) &&
		!(previousStatus & GameInputDeviceConnected))
	{
		for (usize i = 0; i < MAX_CONTROLLERS; i++)
		{
			if (s_Gamepads[i]) continue;

			s_Gamepads[i] = device;
			device->AddRef();
			s_ControllersState[i] = GameInputGamepadNone;
		}
	}
	// disconnected
	if (!(currentStatus & GameInputDeviceConnected) &&
		(previousStatus & GameInputDeviceConnected))
	{
		for (usize i = 0; i < MAX_CONTROLLERS; i++)
		{
			if (s_Gamepads[i] != device) continue;

			device->Release();
			s_Gamepads[i] = nullptr;
			s_ControllersState[i] = GameInputGamepadNone;
		}
	}
}

namespace Input
{
	void Init()
	{
		HRESULT result = GameInputCreate(&s_GameInput);
		if (FAILED(result))
		{
			throw std::runtime_error{ "Failed to initialize GameInput" };
		}

		result = s_GameInput->RegisterDeviceCallback(
			nullptr,
			GameInputKindGamepad,
			GameInputDeviceAnyStatus,
			GameInputNoEnumeration,
			nullptr,
			DeviceStatusCallback,
			&s_CallbackToken
		);

		if (FAILED(result))
		{
			throw std::runtime_error{ "Failed to set GameInput device callback" };
		}
	}

	void Shutdown()
	{
		if (s_CallbackToken)
		{
			s_GameInput->UnregisterCallback(s_CallbackToken);
			s_CallbackToken = 0;
		}

		for (IGameInputDevice*& gamepad : s_Gamepads)
		{
			if (gamepad)
			{
				gamepad->Release();
				gamepad = nullptr;
			}
		}

		if (s_GameInput)
		{
			s_GameInput->Release();
			s_GameInput = nullptr;
		}
	}


	void PollEvents()
	{
		Win32PollEvents();
	}

	bool IsPressed(KeyCode keyCode)
	{
		return s_KeyboardState.test(static_cast<usize>(keyCode));
	}

	bool IsPressed(GamepadButton button, int controllerIndex)
	{
		if (controllerIndex < 0 || controllerIndex >= MAX_CONTROLLERS)
		{
			return false;
		}

		const GameInputGamepadButtons buttons = s_ControllersState[controllerIndex];
		return buttons & AbstractToGameInput(button);
	}

	bool ControllerConnected(int index)
	{
		if (index < 0 || index >= MAX_CONTROLLERS) return false;

		return s_ControllersState[index];
	}
}

