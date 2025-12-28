#pragma once

#include "Common.h"

class Window;

// TODO: hard coded Win32 codes, change this
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

enum class GamepadButton : u32
{
    Null,

    DPadUp = 1u << 1,
    DPadDown = 1u << 2,
    DPadLeft = 1u << 3,
    DPadRight = 1u << 4,

    Start = 1u << 5,
    Back = 1u << 6,

    LeftThumb = 1u << 7,
    RightThumb = 1u << 8,

    LeftShoulder = 1u << 9,
    RightShoulder = 1u << 10,

    LeftTrigger = 1u << 11,
    RightTrigger = 1u << 12,

    A = 1u << 13,
    B = 1u << 14,
    X = 1u << 15,
    Y = 1u << 16,

    LeftStickUp = 1u << 17,
    LeftStickDown = 1u << 18,
    LeftStickLeft = 1u << 19,
    LeftStickRight = 1u << 20,

    RightStickUp = 1u << 21,
    RightStickDown = 1u << 22,
    RightStickLeft = 1u << 23,
    RightStickRight = 1u << 24,
};

enum class KeyEventType
{
    Down,
    Up,
    Repeat,
    Char
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

namespace Input
{
    void SetFocus(const Window& window);

    void PollEvents();

    bool IsPressed(KeyCode code);

    bool IsPressed(GamepadButton button, int controllerIndex = 0);

    bool ControllerConnected(int index);
}