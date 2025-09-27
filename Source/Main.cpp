#include <iostream>

#include <memory>
#include <filesystem>
#include "System.h"

int main()
{
	using namespace std::filesystem;

	auto system = std::make_unique<System>();
	system->Init();

	system->LoadROM("C:\\Users\\shane\\source\\repos\\NES-Emulator\\nestest.nes");

	while (true)
	{
		system->PerformCycle();
	}

	return 0;
}
