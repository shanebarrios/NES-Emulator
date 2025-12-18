#pragma once

#include "../NES/NES.h"
#include <memory>
#include "Window.h"
#include "Common.h"

class Application
{
public:
	Application();
	~Application();

	void Run();

private:
	void LoadPalette(const std::filesystem::path& path, int num = 0);

	void OnKeyEvent(KeyEvent event);

	void OnMouseMoveEvent(MouseMoveEvent event);

	void OnRender();

private:
	std::unique_ptr<NES> m_Nes = nullptr;
	// TODO: change this to abstract platform layer
	Array<WindowPixel, 64> m_SystemPalette{};

	Window m_Window{};
};