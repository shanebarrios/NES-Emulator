#include "Window.h"

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

	case WM_DESTROY:
	{
		m_ShouldQuit = true;
		PostQuitMessage(0);
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		const HDC hdc = BeginPaint(m_Hwnd, &ps);
		//const LONG x = ps.rcPaint.left;
		//const LONG y = ps.rcPaint.right;
		//const LONG width = ps.rcPaint.right - ps.rcPaint.left;
		//const LONG height = ps.rcPaint.bottom - ps.rcPaint.top;
		StretchDIBits(hdc,
			0, 0, m_Width, m_Height,
			0, 0, m_RenderWidth, m_RenderHeight,
			m_BitmapMemory, &m_BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		EndPaint(m_Hwnd, &ps);
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