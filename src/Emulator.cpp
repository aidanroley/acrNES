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
	// The beginning
	emulator.start();

	return 0;

}
void Emulator::start() {
	int val = romMain.parseFile();
	PPU->checkPpuBus();
	
	while (ok) {
		SDL_RenderPresent(renderer);

	}
	SDL_Quit();
}