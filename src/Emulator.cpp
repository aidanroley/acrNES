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
    if (!emulator.ok) {
        return -1;
    }
    emulator.start();
    return 0;
}

void Emulator::start() {
    int val = romMain.parseFile();
    PPU->checkPpuBus();
    PPU->InitializeColors();

    SDL_Event e;
    while (ok) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                ok = false;
            }
        }
        // Example of setting a pixel
        // SetPixel(100, 100, 0xFFFF0000); // Set pixel at (100, 100) to red

       // UpdateScreen();
        bus->busClock();
    }
}
