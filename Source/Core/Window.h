#pragma once

#include "Common.h"
#include "WindowsCommon.h"
#include <string>
#include <span>
#include <memory>
#include <bitset>
#include <functional>
#include <utility>

struct WindowSpec
{
	std::string title;
	int width;
	int height;
	int renderWidth;
	int renderHeight;
};

struct WindowPixel
{
	u8 b;
	u8 g;
	u8 r;
	u8 a;
};

enum class KeyEventType
{
	Down,
    Up,
    Repeat,
    Char
};

enum class KeyCode
{
    Null = 0x00,
    LeftMouse = 0x01,
    RightMouse = 0x02,
    Cancel = 0x03,
    MiddleMouse = 0x04,
    X1Mouse = 0x05,
    X2Mouse = 0x06,
    Backspace = 0x08,
    Tab = 0x09,
    Clear = 0x0C,
    Enter = 0x0D,
    Shift = 0x10,
    Ctrl = 0x11,
    Alt = 0x12,
    Pause = 0x13,
    CapsLock = 0x14,
    Esc = 0x1B,
    Space = 0x20,
    PageUp = 0x21,
    PageDown = 0x22,
    End = 0x23,
    Home = 0x24,
    LeftArrow = 0x25,
    UpArrow = 0x26,
    RightArrow = 0x27,
    DownArrow = 0x28,
    Select = 0x29,
    Print = 0x2A,
    Execute = 0x2B,
    PrintScreen = 0x2C,
    Insert = 0x2D,
    Delete = 0x2E,
    Help = 0x2F,
    Key0 = 0x30,
    Key1 = 0x31,
    Key2 = 0x32,
    Key3 = 0x33,
    Key4 = 0x34,
    Key5 = 0x35,
    Key6 = 0x36,
    Key7 = 0x37,
    Key8 = 0x38,
    Key9 = 0x39,
    A = 0x41,
    B = 0x42,
    C = 0x43,
    D = 0x44,
    E = 0x45,
    F = 0x46,
    G = 0x47,
    H = 0x48,
    I = 0x49,
    J = 0x4A,
    K = 0x4B,
    L = 0x4C,
    M = 0x4D,
    N = 0x4E,
    O = 0x4F,
    P = 0x50,
    Q = 0x51,
    R = 0x52,
    S = 0x53,
    T = 0x54,
    U = 0x55,
    V = 0x56,
    W = 0x57,
    X = 0x58,
    Y = 0x59,
    Z = 0x5A,
    LeftWin = 0x5B,
    RightWin = 0x5C,
    Apps = 0x5D,
    Sleep = 0x5F,
    Numpad0 = 0x60,
    Numpad1 = 0x61,
    Numpad2 = 0x62,
    Numpad3 = 0x63,
    Numpad4 = 0x64,
    Numpad5 = 0x65,
    Numpad6 = 0x66,
    Numpad7 = 0x67,
    Numpad8 = 0x68,
    Numpad9 = 0x69,
    Multiply = 0x6A,
    Add = 0x6B,
    Separator = 0x6C,
    Subtract = 0x6D,
    Decimal = 0x6E,
    Divide = 0x6F,
    F1 = 0x70,
    F2 = 0x71,
    F3 = 0x72,
    F4 = 0x73,
    F5 = 0x74,
    F6 = 0x75,
    F7 = 0x76,
    F8 = 0x77,
    F9 = 0x78,
    F10 = 0x79,
    F11 = 0x7A,
    F12 = 0x7B,
    NumLock = 0x90,
    ScrollLock = 0x91,
    LeftShift = 0xA0,
    RightShift = 0xA1,
    LeftCtrl = 0xA2,
    RightCtrl = 0xA3,
    LeftAlt = 0xA4,
    RightAlt = 0xA5
};

struct KeyEvent
{
    KeyEventType type;
    KeyCode code;
};

struct MouseMoveEvent
{
    int x;
    int y;
};

using KeyCallback = std::function<void(KeyEvent)>;

using MouseMoveCallback = std::function<void(MouseMoveEvent)>;

class Window
{
public:
    Window() = default;
    explicit Window(const WindowSpec& spec);
    ~Window();

    void Init(const WindowSpec& spec);

    HWND GetHandle() const { return m_Hwnd; }

    void Present();

    bool ShouldQuit() const { return m_ShouldQuit; }

    void SetKeyCallback(KeyCallback callback);

    void SetMouseMoveCallback(MouseMoveCallback callback);

    bool IsKeyDown(KeyCode code) const;

    std::pair<int, int> GetMousePos() const;

    std::span<WindowPixel> GetFramebuffer() { return { m_BitmapMemory, static_cast<usize>(m_RenderWidth * m_RenderHeight) }; }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void UpdateWindowSurface(HDC deviceContext, int x, int y, int width, int height);

private:
    HWND m_Hwnd = nullptr;
    BITMAPINFO m_BitmapInfo{};
    WindowPixel* m_BitmapMemory = nullptr;

    std::bitset<256> m_Keys{};
    MouseMoveCallback m_MouseMoveCallback{};
    KeyCallback m_KeyCallback{};

	int m_Width = 0;
	int m_Height = 0;
	int m_RenderWidth = 0;
	int m_RenderHeight = 0;

    int m_MousePosX = 0;
    int m_MousePosY = 0;

    bool m_ShouldQuit = false;
};