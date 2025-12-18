#include "Window.h"

#include <Windowsx.h>

extern HINSTANCE g_hInstance;

static Window* GetWindowFromHandle(HWND hwnd)
{
	LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	return reinterpret_cast<Window*>(ptr);
}

void Window::UpdateWindowSurface(HDC deviceContext, int x, int y, int width, int height)
{
	StretchDIBits(deviceContext,
		0, 0, m_Width, m_Height,
		0, 0, m_RenderWidth, m_RenderHeight,
		m_BitmapMemory, &m_BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window* pThis = nullptr;

	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pThis = reinterpret_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		pThis->m_Hwnd = hwnd;
	}
	else
	{
		pThis = GetWindowFromHandle(hwnd);
	}

	if (pThis)
		return pThis->HandleMessage(uMsg, wParam, lParam);
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
	{
		RECT clientRect;
		GetClientRect(m_Hwnd, &clientRect);
		int width = clientRect.right - clientRect.left;
		int height = clientRect.bottom - clientRect.top;
		m_Width = width;
		m_Height = height;

		return 0;
	}
	case WM_CLOSE:
	{
		m_ShouldQuit = true;
		return 0;
	}
	case WM_DESTROY:
	{
		m_ShouldQuit = true;
		PostQuitMessage(0);
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(m_Hwnd, &ps);
		EndPaint(m_Hwnd, &ps);
		return 0;
	}
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		const WORD keyFlags = HIWORD(lParam);
		const WORD scanCode = LOBYTE(keyFlags);
		const bool repeat = (keyFlags & KF_REPEAT) == KF_REPEAT;
		const bool keyDown = (keyFlags & KF_UP) != KF_UP;
		WORD vkCode = LOWORD(wParam);

		// vkCode does not automatically distinguish these keys
		switch (vkCode)
		{
		case VK_SHIFT:
		case VK_CONTROL:
		case VK_MENU:
			vkCode = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
			break;
		}

		const KeyCode code = static_cast<KeyCode>(vkCode);
		KeyEventType type = keyDown ? KeyEventType::Down : KeyEventType::Up;
		if (repeat && keyDown)
		{
			type = KeyEventType::Repeat;
		}
		m_Keys.set(static_cast<usize>(code), keyDown);

		if (m_KeyCallback)
		{
			m_KeyCallback({ .type = type, .code = code,  });
		}
		return 0;
	}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	{
		KeyCode code = KeyCode::Null;

		if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
		{
			code = KeyCode::LeftMouse;
		}
		else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
		{
			code = KeyCode::MiddleMouse;
		}
		else if (uMsg == WM_RBUTTONUP || uMsg == WM_RBUTTONDOWN)
		{
			code = KeyCode::RightMouse;
		}

		const bool keyDown = 
			uMsg == WM_LBUTTONDOWN || 
			uMsg == WM_MBUTTONDOWN || 
			uMsg == WM_RBUTTONDOWN;
		const KeyEventType type = keyDown ? KeyEventType::Down : KeyEventType::Up;

		m_Keys.set(static_cast<usize>(code), keyDown);

		if (m_KeyCallback)
		{
			m_KeyCallback({ .type = type, .code = code });
		}

		return 0;
	}
	case WM_MOUSEMOVE:
	{
		const int x = GET_X_LPARAM(lParam);
		const int y = GET_Y_LPARAM(lParam);

		if (m_MouseMoveCallback)
		{
			m_MouseMoveCallback({ .x = x, .y = y });
		}

		return 0;
	}

	default:
		return DefWindowProc(m_Hwnd, uMsg, wParam, lParam);
	}
}

Window::Window(const WindowSpec& spec)
{
	Init(spec);
}

Window::~Window()
{
	delete[] m_BitmapMemory;
	DestroyWindow(m_Hwnd);
}

void Window::Init(const WindowSpec& spec)
{
	m_Width = spec.width;
	m_Height = spec.height;
	m_RenderWidth = spec.renderWidth;
	m_RenderHeight = spec.renderHeight;

	constexpr const wchar_t* CLASS_NAME = L"BasicWindow";
	const WNDCLASS wc
	{
		.lpfnWndProc = Window::WindowProc,
		.hInstance = g_hInstance,
		.lpszClassName = CLASS_NAME
	};

	RegisterClass(&wc);

	m_Hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"NES Emulator",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, m_Width, m_Height,
		nullptr,
		nullptr,
		g_hInstance,
		this
	);

	const BITMAPINFOHEADER bitmapHeader
	{
		.biSize = sizeof(bitmapHeader),
		.biWidth = m_RenderWidth,
		.biHeight = m_RenderHeight,
		.biPlanes = 1,
		.biBitCount = 32,
		.biCompression = BI_RGB
	};

	m_BitmapInfo =
	{
		.bmiHeader = bitmapHeader
	};

	m_BitmapMemory = new WindowPixel[m_RenderWidth * m_RenderHeight];

	ShowWindow(m_Hwnd, SW_SHOWNORMAL);
}

void Window::Present()
{
	const HDC hdc = GetDC(m_Hwnd);
	StretchDIBits(hdc,
		0, 0, m_Width, m_Height,
		0, 0, m_RenderWidth, m_RenderHeight,
		m_BitmapMemory, &m_BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(m_Hwnd, hdc);
}

void Window::SetKeyCallback(KeyCallback callback)
{
	m_KeyCallback = callback;
}

void Window::SetMouseMoveCallback(MouseMoveCallback callback)
{
	m_MouseMoveCallback = callback;
}

bool Window::IsKeyDown(KeyCode code) const
{
	return m_Keys[static_cast<usize>(code)];
}

std::pair<int, int> Window::GetMousePos() const
{
	return { m_MousePosX, m_MousePosY };
}