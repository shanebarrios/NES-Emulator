#pragma once

#include "Common.h"
#include "WindowsCommon.h"
#include "Input.h"
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

    std::span<WindowPixel> GetFramebuffer() { return { m_BitmapMemory, static_cast<usize>(m_RenderWidth * m_RenderHeight) }; }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void UpdateWindowSurface(HDC deviceContext, int x, int y, int width, int height);

private:
    HWND m_Hwnd = nullptr;
    BITMAPINFO m_BitmapInfo{};
    HBITMAP m_BackBitmap{};
    HDC m_BackDC{};

    WindowPixel* m_BitmapMemory = nullptr;

	int m_Width = 0;
	int m_Height = 0;
	int m_RenderWidth = 0;
	int m_RenderHeight = 0;

    bool m_ShouldQuit = false;
};