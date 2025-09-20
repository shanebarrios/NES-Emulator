#include <iostream>

#include <memory>
#include <filesystem>
#include "CPU.h"
#include "Bus.h"

int main()
{
	using namespace std::filesystem;

	auto bus = std::make_unique<Bus>();

	bus->LoadROM("C:\\Users\\shane\\source\\repos\\NES-Emulator\\nestest.nes");
	
	CPU cpu{ *bus };

	while (true)
	{
		cpu.PerformCycle();
	}

	return 0;
}
