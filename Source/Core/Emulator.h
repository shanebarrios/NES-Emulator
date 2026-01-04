#pragma once

#include "../NES/NES.h"
#include "Common.h"
#include "VirtualController.h"
#include "Window.h"
#include <memory>

enum class NESButton;

class Emulator
{
  public:
    Emulator();
    ~Emulator();

    void Run();

  private:
    bool LoadPalette(const std::filesystem::path& path, int num = 0);

    void UpdateInput();

    void OnRender();

  private:
    std::unique_ptr<NES> m_Nes = nullptr;
    // TODO: change this to abstract platform layer
    Array<WindowPixel, 64> m_SystemPalette{};
    VirtualController m_Controller{};

    Window m_Window{};
};
