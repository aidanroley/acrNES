#include "PPU.h"
#include "ROM.h"
#include "cpu.h"
#include "Emulator.h"

#include <SDL.h>
#include <iostream>
#include <chrono>
#include <thread>

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

    const double frameDuration = 1.0 / 60.0; // Duration of one frame in seconds
    const auto frameDurationMs = std::chrono::milliseconds(static_cast<int>(frameDuration * 1000)); // Duration of one frame in milliseconds

    while (ok) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        PPU->frameDone = false;
        doInput = true;
        bus->inputRegister = 0;
 
        // Run the PPU and CPU until a frame is completed
        while (!PPU->frameDone) {
          

            if (doInput && bus->ppuCycles % 5 == 0) {
                
                    SDL_Event e;
                    if (SDL_PollEvent(&e) != 0) {
                        if (e.type == SDL_QUIT) {
                            ok = false;
                        }
                    } 
                    
                const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
                bus->updateControllerState(currentKeyStates);
            }
            bus->busClock();
        }
        
        // Calculate how long the frame took
        auto frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedTime = frameEnd - frameStart;

        // Calculate the time to sleep to maintain 60 FPS
        auto sleepDuration = frameDurationMs - std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime);
        if (sleepDuration > std::chrono::milliseconds(0)) {
            std::this_thread::sleep_for(sleepDuration);
        }
        
    }
}