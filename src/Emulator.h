#include <SDL.h>
#include "PPU.h"
#include "ROM.h"
class Emulator {
private:
	SDL_Window* window;      // Window for display
	SDL_Renderer* renderer;  // Renderer for graphical output
	ROM rom;                 // ROM handling object
public:

	PPU* PPU = PPU::getInstance();
	bool ok = true;

	Emulator() : window(nullptr), renderer(nullptr), PPU(PPU::getInstance()) {}

    void SetUpDisplay() {
        // Initialize display and renderer and respective sizes/scaling
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("Scaled 256 x 240 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 480, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        SDL_PixelFormat* format = SDL_AllocFormat(SDL_GetWindowPixelFormat(window));
        SDL_RenderSetLogicalSize(renderer, 256, 240);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawPoint(renderer, 100, 100);
        SDL_RenderPresent(renderer);

        // Pass renderer reference to PPU class
        PPU->InitializeRenderer(renderer, format);
    }

    void start();

    ~Emulator() {
        // Clean up resources
        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        SDL_Quit();
    }
};