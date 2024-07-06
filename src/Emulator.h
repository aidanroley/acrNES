#ifndef EMULATOR_H
#define EMULATOR_H

#include <SDL.h>
#include "PPU.h"
#include "ROM.h"
#include <vector>
#include <iostream>

class Emulator {
private:
    SDL_Window* window;      // Window for display
    SDL_Renderer* renderer;  // Renderer for graphical output
    SDL_Texture* texture;    // Texture for rendering pixels
    SDL_PixelFormat* format;
    std::vector<uint32_t> pixelBuffer; // Buffer for pixel data
    ROM rom;                 // ROM handling object

public:
    PPU* PPU = PPU::getInstance();
    Bus* bus = Bus::getInstance();
    bool ok = true;

    float time;

    Emulator() : window(nullptr), renderer(nullptr), texture(nullptr), PPU(PPU::getInstance()) {
        pixelBuffer.resize(256 * 240);
    }

    void SetUpDisplay() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            ok = false;
            return;
        }
        window = SDL_CreateWindow("Scaled 256 x 240 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 480, SDL_WINDOW_SHOWN);
        if (!window) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            ok = false;
            return;
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            ok = false;
            return;
        }
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);
        if (!texture) {
            std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            ok = false;
            return;
        }

        SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
        format = screenSurface->format;

        PPU->InitializeRenderer(renderer, format, texture);
    }

    void UpdateScreen() {
        SDL_UpdateTexture(texture, nullptr, pixelBuffer.data(), 256 * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    void SetPixel(int x, int y, uint32_t color) {
        if (x >= 0 && x < 256 && y >= 0 && y < 240) {
            pixelBuffer[y * 256 + x] = color;
        }
    }

    void start();

    ~Emulator() {
        // Clean up resources
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
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

#endif // EMULATOR_H
