#include "Application.h"
#include "Logger.h"
#include "../NES/PPU.h"

#include <thread>
#include <fstream>

static void GlobalInit()
{
	g_Logger.Init();
	g_Logger.SetLogLevel(LogLevel::Info);
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

Application::Application()
{
	GlobalInit();
	m_Nes = std::make_unique<NES>();
	const WindowSpec windowSpec
	{
		.title = "NES Emulator",
		.width = 1280, .height = 1200,
		.renderWidth = 256, .renderHeight = 240
	};
	m_Window.Init(windowSpec);

	LoadPalette("C:\\Users\\shane\\source\\repos\\NES-Emulator\\Assets\\Palettes\\2C02G_wiki.pal");
	m_Nes->LoadROM("C:\\Users\\shane\\source\\repos\\NES-Emulator\\donkey_kong.nes");
	m_Nes->Reset();
}

Application::~Application()
{
	
}

void Application::LoadPalette(const std::filesystem::path& path, int num)
{
	std::ifstream inf{ path, std::ios::binary };

	if (!inf)
		throw std::runtime_error{ "Failed to load system palette file" };

	constexpr usize READ_PALETTE_SIZE = SYSTEM_PALETTE_LENGTH * 3;
	Array<char, READ_PALETTE_SIZE> buf;

	inf.seekg(num * (READ_PALETTE_SIZE), std::ios::beg);
	if (!inf)
		throw std::runtime_error{ "System palette offset out of bounds" };

	inf.read(buf.data(), buf.size());

	if (inf.gcount() != buf.size())
		throw std::runtime_error{ "Invalid system palette file" };

	for (usize i = 0; i < m_SystemPalette.size(); i++)
	{
		m_SystemPalette[i].r = buf[i * 3];
		m_SystemPalette[i].g = buf[i * 3 + 1];
		m_SystemPalette[i].b = buf[i * 3 + 2];
	}
}

void Application::Render()
{
	auto windowFramebuffer = m_Window.GetFramebuffer();
	const u8* ppuFramebuffer = m_Nes->GetPPU().GetFramebuffer();
	for (usize y = 0; y < PPU::SCREEN_HEIGHT; y++)
	{
		const usize srcRow = y;
		const usize dstRow = PPU::SCREEN_HEIGHT - y - 1;

		for (usize x = 0; x < PPU::SCREEN_WIDTH; x++)
		{
			const usize srcIndex = srcRow * PPU::SCREEN_WIDTH + x;
			const usize dstIndex = dstRow * PPU::SCREEN_WIDTH + x;
			const WindowPixel color = m_SystemPalette[ppuFramebuffer[srcIndex]];
			windowFramebuffer[dstIndex] = color;
		}
	}
	m_Window.Present();
	m_Nes->GetPPU().ClearFramebufferReady();
}

void Application::Run()
{
	using namespace std::chrono;
	using clock = high_resolution_clock;

	constexpr u64 frameTimeNS = static_cast<u64>(NES::FRAME_TIME * 1000000000);

	auto nextFrame = clock::now();
	auto lastSec = nextFrame;
	double frameTimeTotal = 0.0;
	int frames = 0;

	while (!m_Window.ShouldQuit())
	{
		auto frameBegin = clock::now();

		nextFrame += nanoseconds(frameTimeNS);

		PollEvents();
		while (!m_Nes->GetPPU().FramebufferReady())
		{
			m_Nes->Update();
		}

		Render();

		auto frameEnd = clock::now();

		const double frameTimeReal = duration<double>(frameEnd - frameBegin).count();
		frameTimeTotal += frameTimeReal;
		frames++;

		if (frameEnd > lastSec)
		{
			lastSec += seconds(1);

			LOG_INFO("Frame time avg: %f, fps: %d", frameTimeTotal / frames, frames);
			frames = 0;
			frameTimeTotal = 0.0;
		}


		std::this_thread::sleep_until(nextFrame);
	}
}