#include "Input.h"

#include "Logger.h"
#include <bitset>
#include <vector>
#include "GameInput.h"
#include "../Core/Logger.h"

using namespace GameInput::v3;
static constexpr usize MAX_CONTROLLERS = 4;

typedef HRESULT (WINAPI *PFN_GameInputInitialize)(
	_In_ REFIID riid,
	_COM_Outptr_ LPVOID* ppv
	);  

struct
{
	bool initialized = false;
	HMODULE dll = nullptr;
	IGameInput* gameInput = nullptr;
	Array<IGameInputDevice*, MAX_CONTROLLERS> gamepads{};
	GameInputCallbackToken callbackToken = 0;
} s_GameInput;

struct ControllerInput
{
	u32 buttons = 0;
	bool connected = false;
};

struct
{
	Array<ControllerInput, MAX_CONTROLLERS> controllerInput{};
	std::bitset<256> keyboardInput{};
	u64 mouseX = 0;
	u64 mouseY = 0;
} s_InputState;

static GamepadButton GameInputToAbstract(GameInputGamepadButtons button)
{
	switch (button)
	{
	case GameInputGamepadNone:                    return GamepadButton::Null;
	case GameInputGamepadDPadDown:                return GamepadButton::DPadDown;
	case GameInputGamepadDPadLeft:                return GamepadButton::DPadLeft;
	case GameInputGamepadDPadRight:               return GamepadButton::DPadRight;
	case GameInputGamepadMenu:                    return GamepadButton::Start;
	case GameInputGamepadView:                    return GamepadButton::Back;
	case GameInputGamepadLeftThumbstick:           return GamepadButton::LeftThumb;
	case GameInputGamepadRightThumbstick:          return GamepadButton::RightThumb;
	case GameInputGamepadLeftShoulder:             return GamepadButton::LeftShoulder;
	case GameInputGamepadRightShoulder:            return GamepadButton::RightShoulder;
	case GameInputGamepadA:                        return GamepadButton::A;
	case GameInputGamepadB:                        return GamepadButton::B;
	case GameInputGamepadX:                        return GamepadButton::X;
	case GameInputGamepadY:                        return GamepadButton::Y;
	case GameInputGamepadLeftThumbstickUp:         return GamepadButton::LeftStickUp;
	case GameInputGamepadLeftThumbstickDown:       return GamepadButton::LeftStickDown;
	case GameInputGamepadLeftThumbstickLeft:       return GamepadButton::LeftStickLeft;
	case GameInputGamepadLeftThumbstickRight:      return GamepadButton::LeftStickRight;
	case GameInputGamepadRightThumbstickUp:        return GamepadButton::RightStickUp;
	case GameInputGamepadRightThumbstickDown:      return GamepadButton::RightStickDown;
	case GameInputGamepadRightThumbstickLeft:      return GamepadButton::RightStickLeft;
	case GameInputGamepadRightThumbstickRight:     return GamepadButton::RightStickRight;
	default:                                       return GamepadButton::Null;
	}
}

static void Win32PollInput()
{
	for (usize i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (!s_GameInput.gamepads[i]) continue;

		IGameInputReading* reading;
		if (FAILED(s_GameInput.gameInput->GetCurrentReading(
			GameInputKindGamepad,
			s_GameInput.gamepads[i],
			&reading)))
		{
			continue;
		}

		GameInputGamepadState gameInputState;
		reading->GetGamepadState(&gameInputState);
		reading->Release();

		u32 abstractState = 0;

		for (u32 shift = 0; shift < 32; shift++)
		{
			const auto mask = static_cast<GameInputGamepadButtons>(1 << shift);
			const GamepadButton button = GameInputToAbstract(mask);
			if (button != GamepadButton::Null && (gameInputState.buttons & mask))
			{
				abstractState |= static_cast<u32>(button);
			}
		}

		s_InputState.controllerInput[i].buttons = abstractState;

	}

	IGameInputReading* reading;
	s_InputState.keyboardInput.reset();
	if (SUCCEEDED(s_GameInput.gameInput->GetCurrentReading(
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
			s_InputState.keyboardInput.set(state.virtualKey);
		}

		GameInputMouseState mouseState;
		if (reading->GetMouseState(&mouseState))
		{
			s_InputState.mouseX = mouseState.absolutePositionX;
			s_InputState.mouseY = mouseState.absolutePositionY;
			s_InputState.keyboardInput.set(
				static_cast<usize>(KeyCode::LeftMouse),
				mouseState.buttons & GameInputMouseLeftButton);
			s_InputState.keyboardInput.set(
				static_cast<usize>(KeyCode::RightMouse),
				mouseState.buttons & GameInputMouseRightButton);
			s_InputState.keyboardInput.set(
				static_cast<usize>(KeyCode::MiddleMouse),
				mouseState.buttons & GameInputMouseMiddleButton);
			s_InputState.keyboardInput.set(
				static_cast<usize>(KeyCode::Mouse4),
				mouseState.buttons & GameInputMouseButton4);
			s_InputState.keyboardInput.set(
				static_cast<usize>(KeyCode::Mouse5),
				mouseState.buttons & GameInputMouseButton5);
		}
		reading->Release();
	}
}

static void Win32PollInputFallback()
{
	s_InputState.keyboardInput.reset();
	Array<u8, 256> buf;
	if (!GetKeyboardState(buf.data()))
		return;
	for (usize i = 0; i < buf.size(); i++)
	{
		const u8 key = buf[i];
		s_InputState.keyboardInput.set(i, key & 0x80);
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

	if (s_GameInput.initialized)
		Win32PollInput();
	else
		Win32PollInputFallback();
}

static void Win32DeviceStatusCallback(
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
			if (s_GameInput.gamepads[i]) continue;

			s_GameInput.gamepads[i] = device;
			device->AddRef();
			s_InputState.controllerInput[i].connected = true;
			LOG_INFO("Controller %d connected", i);
			break;
		}
	}
	// disconnected
	if (!(currentStatus & GameInputDeviceConnected) &&
		(previousStatus & GameInputDeviceConnected))
	{
		for (usize i = 0; i < MAX_CONTROLLERS; i++)
		{
			if (s_GameInput.gamepads[i] != device) continue;

			device->Release();
			s_GameInput.gamepads[i] = nullptr;
			s_InputState.controllerInput[i].buttons = 0;
			s_InputState.controllerInput[i].connected = false;
			LOG_WARN("Controller %d disconnected", i);
			break;
		}
	}
}

static bool Win32InputInit()
{
	const HMODULE dll = LoadLibraryW(L"GameInputRedist.dll");
	if (!dll)
	{
		LOG_ERROR("Failed to load GameInputRedist.dll");
		return false;
	}
	auto initFun = (PFN_GameInputInitialize) GetProcAddress(dll, "GameInputInitialize");
	if (initFun == nullptr)
	{
		LOG_ERROR("Failed to get address of GameInputInitialize");
		FreeLibrary(dll);
		return false;
	}

	HRESULT result = initFun(IID_IGameInput, reinterpret_cast<void**>(&s_GameInput.gameInput));
	if (FAILED(result))
	{
		LOG_ERROR("Failed to initialize game input, error code %d", result);
		FreeLibrary(dll);
		return false;
	}

	result = s_GameInput.gameInput->RegisterDeviceCallback(
		nullptr,
		GameInputKindGamepad,
		GameInputDeviceAnyStatus,
		GameInputBlockingEnumeration,
		nullptr,
		Win32DeviceStatusCallback,
		&s_GameInput.callbackToken
	);

	if (FAILED(result))
	{
		LOG_ERROR("Failed to create GameInput device callback");
		FreeLibrary(dll);
		return false;
	}

	s_GameInput.initialized = true;
	s_GameInput.dll = dll;
	return true;
}

static void Win32InputShutdown()
{
	if (s_GameInput.callbackToken)
	{
		s_GameInput.gameInput->UnregisterCallback(s_GameInput.callbackToken);
		s_GameInput.callbackToken = 0;
	}

	for (IGameInputDevice*& gamepad : s_GameInput.gamepads)
	{
		if (gamepad)
		{
			gamepad->Release();
			gamepad = nullptr;
		}
	}

	if (s_GameInput.gameInput)
	{
		s_GameInput.gameInput->Release();
		s_GameInput.gameInput = nullptr;
	}

	FreeLibrary(s_GameInput.dll);
	s_GameInput.dll = nullptr;
	s_GameInput.initialized = false;
}

namespace Input
{
	bool Init()
	{
		return Win32InputInit();
	}

	void Shutdown()
	{
		Win32InputShutdown();
	}

	void PollEvents()
	{
		Win32PollEvents();
	}

	bool IsPressed(KeyCode keyCode)
	{
		return s_InputState.keyboardInput.test(static_cast<usize>(keyCode));
	}

	bool IsPressed(GamepadButton button, int controllerIndex)
	{
		if (controllerIndex < 0 || controllerIndex >= MAX_CONTROLLERS)
		{
			return false;
		}

		const u32 buttons = s_InputState.controllerInput[controllerIndex].buttons;
		return buttons & static_cast<u32>(button);
	}

	bool ControllerConnected(int index)
	{
		if (index < 0 || index >= MAX_CONTROLLERS) return false;

		return s_InputState.controllerInput[index].connected;
	}
}

