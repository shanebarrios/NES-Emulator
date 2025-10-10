#include "WindowsCommon.h"
#include <memory>
#include <ShObjIdl.h>
#include <atlbase.h>
#include <filesystem>
#include "System.h"
#include <stdio.h>
#include <thread>
#include <chrono>
#include "Window.h"

#include <cstdint>

HINSTANCE g_hInstance = nullptr;

static int Main();

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
	AllocConsole();
	FILE* dummy;
	freopen_s(&dummy, "CONOUT$", "w", stdout);
	freopen_s(&dummy, "CONOUT$", "w", stderr);
	freopen_s(&dummy, "CONOUT$", "r", stdin);

	g_hInstance = hInstance;

	return Main();
}

void PollEvents()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

#include <cassert>

int Main()
{
	assert(false);
	auto system = std::make_unique<System>();
	const WindowSpec windowSpec
	{
		.title = "NES Emulator",
		.width = 1920, .height = 1080,
		.renderWidth = 256, .renderHeight = 240
	};
	Window window{ windowSpec };

	std::span<WindowPixel> framebuffer = window.GetFramebuffer();
	for (isize i = 0; i < 240; i++)
	{
		const float u = static_cast<float>(i) / 240.0f;
		const u8 r = static_cast<u8>(u * 0xFF);
		for (isize j = 0; j < 256; j++)
		{
			const float v = static_cast<float>(j) / 256.0f;
			const u8 g = static_cast<u8>(v * 0xFF);
			const u8 b = 0xFF;
			framebuffer[i * 256 + j] =
			{
				.b = b,
				.g = g,
				.r = r,
				.a = 0xFF
			};
		}
	}

	system->Init();
	system->LoadROM("C:\\Users\\shane\\source\\repos\\NES-Emulator\\nestest.nes");

	while (!window.ShouldQuit())
	{
		PollEvents();
		//window.Present();

		system->PerformCycle();

		//std::this_thread::sleep_for(std::chrono::milliseconds(17));
	}

	return 0;
}
