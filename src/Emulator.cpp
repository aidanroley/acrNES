#include "PPU.h"
#include "ROM.h"
#include "cpu.h"
#include "Emulator.h"

#include <SDL.h>
#include <iostream>

Emulator emulator;
ROM romMain;
int main(int argc, char* argv[]) {
	emulator.SetUpDisplay();
	emulator.start();

	return 0;

}
void Emulator::start() {
	int val = romMain.parseFile();
	PPU->checkPpuBus();
	PPU->InitializeColors();
	
	// The system
	while (ok) {
		//SDL_RenderPresent(renderer);
		ok = false;

	}
	SDL_Quit();
}