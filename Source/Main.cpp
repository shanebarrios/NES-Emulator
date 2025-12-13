#include "Core/WindowsCommon.h"
#include "Core/Window.h"
#include "Core/Logger.h"
#include "NES/System.h"
#include <memory>
#include <stdio.h>
#include <chrono>
#include <thread>

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

static void PollEvents()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

static void GlobalInit()
{
	g_Logger.Init();
	g_Logger.SetLogLevel(LogLevel::Verbose);
}

static int Main()
{
	GlobalInit();

	auto system = std::make_unique<System>();
	const WindowSpec windowSpec
	{
		.title = "NES Emulator",
		.width = 1280, .height = 1200,
		.renderWidth = 256, .renderHeight = 240
	};
	Window window{ windowSpec };

	system->LoadPalette("C:\\Users\\shane\\source\\repos\\NES-Emulator\\Assets\\Palettes\\2C02G_wiki.pal");
	system->LoadROM("C:\\Users\\shane\\source\\repos\\NES-Emulator\\nestest.nes");
	system->Reset();

	using namespace std::chrono;
	high_resolution_clock::time_point lastFrame = high_resolution_clock::now();
	auto lastSecond = lastFrame;

	double lag = 0.0;
	u64 loops = 0;

	while (!window.ShouldQuit())
	{
		const high_resolution_clock::time_point currentFrame = high_resolution_clock::now();
		const double deltaTime = duration<double>(currentFrame - lastFrame).count();
		lastFrame = currentFrame;
		lag += deltaTime;

		if (duration_cast<milliseconds>(currentFrame - lastSecond).count() > 1000)
		{
			LOG_INFO("Loops: %ld", loops);
			loops = 0;
			lastSecond += seconds(1);
		}

		//PollEvents();

		constexpr double delay = System::MASTER_CLOCK_PERIOD;
		while (lag >= delay)
		{
			system->Update();
			lag -= delay;
		}
		window.Present();
		loops++;
	}

	return 0;
}
