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
		.width = 1280, .height = 960,
		.renderWidth = 256, .renderHeight = 240
	};
	LoadPalette("C:\\Users\\shane\\source\\repos\\NES-Emulator\\Assets\\Palettes\\2C02G_wiki.pal");
	m_Nes->LoadROM("C:\\Users\\shane\\source\\repos\\NES-Emulator\\donkey kong.nes");
	m_Nes->Reset();

	m_Window.Init(windowSpec);
	m_Window.SetKeyCallback([this](KeyEvent event){
		OnKeyEvent(event);
	});
	m_Window.SetMouseMoveCallback([this](MouseMoveEvent event) {
		OnMouseMoveEvent(event);
	});
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

void Application::OnRender()
{
	auto windowFramebuffer = m_Window.GetFramebuffer();
	const u8* ppuFramebuffer = m_Nes->GetFramebuffer();
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
}

void Application::OnKeyEvent(KeyEvent event)
{
	ControllerButton button;

	switch (event.code)
	{
	case KeyCode::Z:
		button = ControllerButton::B;
		break;
	case KeyCode::X:
		button = ControllerButton::A;
		break;
	case KeyCode::LeftArrow:
		button = ControllerButton::Left;
		break;
	case KeyCode::UpArrow:
		button = ControllerButton::Up;
		break;
	case KeyCode::RightArrow:
		button = ControllerButton::Right;
		break;
	case KeyCode::DownArrow:
		button = ControllerButton::Down;
		break;
	case KeyCode::RightShift:
		button = ControllerButton::Select;
		break;
	case KeyCode::Enter:
		button = ControllerButton::Start;
		break;
	default:
		// No NES key mapping, dont handle the event
		return;
	}

	const bool pressed = 
		event.type == KeyEventType::Down || 
		event.type == KeyEventType::Repeat;

	m_Nes->SetButtonState(button, pressed);
}

void Application::OnMouseMoveEvent(MouseMoveEvent event)
{
	// nothing for now
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
		m_Nes->StepFrame();

		OnRender();

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