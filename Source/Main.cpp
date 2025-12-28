#include "Core/WindowsCommon.h"
#include <stdio.h>
#include "Core/Emulator.h"

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

static int Main()
{
	Emulator emulator {};
	emulator.Run();

	return 0;
}
