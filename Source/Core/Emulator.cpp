#include "Emulator.h"
#include "../NES/PPU.h"
#include "Logger.h"

#include <fstream>
#include <thread>

static void GlobalInit()
{
    g_Logger.Init();
    g_Logger.SetLogLevel(LogLevel::Info);
    Input::Init();
    LOG_INFO("Initialized globals");
}

Emulator::Emulator()
{
    GlobalInit();
    m_Nes = std::make_unique<NES>();
    const WindowSpec windowSpec{
        .title = "NES Emulator", .width = 1280, .height = 960, .renderWidth = 256, .renderHeight = 240};
    LoadPalette("Assets/Palettes/2C02G_wiki.pal");
    // TODO: change this
    if (!m_Nes->LoadROM("Roms/zelda.nes"))
    {
        std::abort();
    }

    m_Nes->Reset();

    m_Window.Init(windowSpec);
    Input::PollEvents();

    if (Input::ControllerConnected(0))
    {
        LOG_INFO("Controller detected, setting default controller keybinds");
        m_Controller.SetDefaultControllerBindings();
    }
    else
    {
        LOG_INFO("No controller connected, setting default keyboard keybindings");
        m_Controller.SetDefaultKeyboardBindings();
    }
}

Emulator::~Emulator()
{
    Input::Shutdown();
}

bool Emulator::LoadPalette(const std::filesystem::path& path, int num)
{
    std::ifstream inf{path, std::ios::binary};
    const std::string pathStr = path.string();

    if (!inf)
    {
        LOG_ERROR("Failed to open system pallette file %s", pathStr.c_str());
        return false;
    }

    constexpr usize READ_PALETTE_SIZE = SYSTEM_PALETTE_LENGTH * 3;
    Array<char, READ_PALETTE_SIZE> buf;

    inf.seekg(num * (READ_PALETTE_SIZE), std::ios::beg);
    if (!inf)
    {
        LOG_ERROR("System palette offset out of bounds");
        return false;
    }

    inf.read(buf.data(), buf.size());

    if (inf.gcount() != buf.size())
    {
        LOG_ERROR("Invalid system palette file");
        return false;
    }

    for (usize i = 0; i < m_SystemPalette.size(); i++)
    {
        m_SystemPalette[i].r = buf[i * 3];
        m_SystemPalette[i].g = buf[i * 3 + 1];
        m_SystemPalette[i].b = buf[i * 3 + 2];
    }
    return true;
}

void Emulator::OnRender()
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

void Emulator::UpdateInput()
{
    Input::PollEvents();
    m_Nes->SetButtonsState(m_Controller.ToHardwareState());
}

void Emulator::Run()
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

        UpdateInput();

        m_Nes->StepFrame();

        OnRender();

        auto frameEnd = clock::now();

        const double frameTimeReal = duration<double>(frameEnd - frameBegin).count();
        frameTimeTotal += frameTimeReal;
        frames++;

        if (frameEnd > lastSec)
        {
            lastSec += seconds(1);

            // LOG_INFO("Frame time avg: %f, fps: %d", frameTimeTotal / frames, frames);
            frames = 0;
            frameTimeTotal = 0.0;
        }

        std::this_thread::sleep_until(nextFrame);
    }
}
