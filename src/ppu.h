#pragma once
#ifndef PPU_H
#define PPU_H
// #include "singleton.h"
#include <cstdint>
#include <SDL.h>
#include "bus.h"


class PPU : public Singleton<PPU> {
	friend class Singleton<PPU>;
private:

	SDL_Renderer* renderer;
	SDL_PixelFormat* format;
	SDL_Texture* texture;
	std::vector<uint32_t> pixelBuffer;
	byte OAM[256]{ 0 };
	uint16_t OAMstartAddr{ 0 };
	int patternTable[0x2000];

public:

	// Pass SDL components into the PPU class
	void InitializeRenderer(SDL_Renderer* render, SDL_PixelFormat* formatted, SDL_Texture* text) {
		this->renderer = render;
		this->format = formatted;
		this->texture = text;
	}

	// To make sure no instances of Bus are created as that would create sync problems
	Bus* bus = Bus::getInstance();

	// Initialize CHR, transfer CHR data from Mapper to PPU
	std::vector<byte> ppuCHR;
	void initializeCHRM0(const std::vector<uint8_t>& chr); // Might change this to vector since it should be 8KB, not yet.
	void getPatternTable(byte address);
	int getPixelValue(byte PT, uint16_t tile_index, byte x, byte y);

	// PPU VRAM
	byte ppuVRAM[0x4000]{ 0 };

	void checkPpuBus();

	// Made 2 variables for nametable orientation to avoid confusion
	bool horizontal = false; 
	bool vertical = false;

	// Determine whether horizontal or vertically oriented nametable
	void mirror(bool mirror);

	// Handle writing/reading OAM
	void writeOAM(byte address, byte value);
	byte readOAM(byte address);

	// This array will store all possible pal pixel colors 
	uint32_t pixelColors[0x40];

	// Flags for PPUCTRL
	struct PPUCTRL {
		bool V = 0; // 7: Generate NMI at start of vertical blanking (0: off, 1: on) 
		bool P = 0; // 6: PPU master/slave select (0: read backdrop from pins, 1: output color on pins)
		bool H = 0; // 5: Sprite size (0: 8x8 pixels, 1: 8x16 pixels)
		bool B = 0; // 4: Background pattern table address (0: $0000, 1: $1000)
		bool S = 0; // 3: Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode)
		bool I = 1; // 2: VRAM address increment per CPU read/write of PPUDATA (0: add 1, going across; 1: add 32, going down)
		byte NN : 2; // 1 | 0: Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
	};
	PPUCTRL PPUCTRL;
	uint16_t TempNametableAddr;

	// Flags for PPUMASK
	struct PPUMASK {
		bool Blue = 0; // 7: Emphasize blue
		bool Green = 0; // 6: Emphasize green
		bool Red = 0; // 5: Emphasize red
		bool s = 0; // 4: Show sprites
		bool b = 0; // 3: Show background
		bool M = 0; // 2: (1: Show sprites in leftmost 8 pixels of the screen, 0: Hide)
		bool m = 0; // 1: (1: Show background in leftmost 8 pixels of screen, 0: Hide)
		bool Greyscale = 0; // 0: Greyscale (0: normal color, 1: produce a greyscale display)
	};
	PPUMASK PPUMASK;

	// Flags for PPUSTATUS
	struct PPUSTATUS {
		bool V = true; // Vertical blank has started
		bool S = 0; // Sprite 0 hit
		bool O = true; // Sprite overflow but its buggy in the NES
	};
	PPUSTATUS PPUSTATUS;

	struct PPUADDR {
		uint16_t lower = 0;
		uint16_t upper = 0;
	};
	PPUADDR PPUADDR;
	uint16_t VRAMaddress = 0;

	bool printDebug = true;

	byte OAMAddr = 0;

	// PPU internal registers
	struct PPUREGISTERS {
		bool w = 0; // SCROLL and ADDR share this register for writing
		uint16_t v; // Current VRAM addr
		uint16_t t; // Temp VRAM addr
		byte x; // Fine scroll

	};
	PPUREGISTERS registers;

	// Handle read/write to PPU Registers
	byte handlePPURead(uint16_t ppuRegister);
	void handlePPUWrite(uint16_t ppuRegister, byte value);

	// RGB values: https://www.nesdev.org/wiki

	void InitializeColors() {
		pixelColors[0x00] = SDL_MapRGB(format, 84, 84, 84);
		pixelColors[0x01] = SDL_MapRGB(format, 0, 30, 116);
		pixelColors[0x02] = SDL_MapRGB(format, 8, 16, 144);
		pixelColors[0x03] = SDL_MapRGB(format, 48, 0, 136);
		pixelColors[0x04] = SDL_MapRGB(format, 68, 0, 100);
		pixelColors[0x05] = SDL_MapRGB(format, 92, 0, 48);
		pixelColors[0x06] = SDL_MapRGB(format, 84, 4, 0);
		pixelColors[0x07] = SDL_MapRGB(format, 60, 24, 0);
		pixelColors[0x08] = SDL_MapRGB(format, 32, 42, 0);
		pixelColors[0x09] = SDL_MapRGB(format, 8, 58, 0);
		pixelColors[0x0A] = SDL_MapRGB(format, 0, 64, 0);
		pixelColors[0x0B] = SDL_MapRGB(format, 0, 60, 0);
		pixelColors[0x0C] = SDL_MapRGB(format, 0, 50, 60);
		pixelColors[0x0D] = SDL_MapRGB(format, 0, 0, 0);
		pixelColors[0x0E] = SDL_MapRGB(format, 0, 0, 0);
		pixelColors[0x0F] = SDL_MapRGB(format, 0, 0, 0);

		pixelColors[0x10] = SDL_MapRGB(format, 152, 150, 152);
		pixelColors[0x11] = SDL_MapRGB(format, 8, 76, 196);
		pixelColors[0x12] = SDL_MapRGB(format, 48, 50, 236);
		pixelColors[0x13] = SDL_MapRGB(format, 92, 30, 228);
		pixelColors[0x14] = SDL_MapRGB(format, 136, 20, 176);
		pixelColors[0x15] = SDL_MapRGB(format, 160, 20, 100);
		pixelColors[0x16] = SDL_MapRGB(format, 152, 34, 32);
		pixelColors[0x17] = SDL_MapRGB(format, 120, 60, 0);
		pixelColors[0x18] = SDL_MapRGB(format, 84, 90, 0);
		pixelColors[0x19] = SDL_MapRGB(format, 40, 114, 0);
		pixelColors[0x1A] = SDL_MapRGB(format, 8, 124, 0);
		pixelColors[0x1B] = SDL_MapRGB(format, 0, 118, 40);
		pixelColors[0x1C] = SDL_MapRGB(format, 0, 102, 120);
		pixelColors[0x1D] = SDL_MapRGB(format, 0, 0, 0);
		pixelColors[0x1E] = SDL_MapRGB(format, 0, 0, 0);
		pixelColors[0x1F] = SDL_MapRGB(format, 0, 0, 0);

		pixelColors[0x20] = SDL_MapRGB(format, 236, 238, 236);
		pixelColors[0x21] = SDL_MapRGB(format, 76, 154, 236);
		pixelColors[0x22] = SDL_MapRGB(format, 120, 124, 236);
		pixelColors[0x23] = SDL_MapRGB(format, 176, 98, 236);
		pixelColors[0x24] = SDL_MapRGB(format, 228, 84, 236);
		pixelColors[0x25] = SDL_MapRGB(format, 236, 88, 180);
		pixelColors[0x26] = SDL_MapRGB(format, 236, 106, 100);
		pixelColors[0x27] = SDL_MapRGB(format, 212, 136, 32);
		pixelColors[0x28] = SDL_MapRGB(format, 160, 170, 0);
		pixelColors[0x29] = SDL_MapRGB(format, 116, 196, 0);
		pixelColors[0x2A] = SDL_MapRGB(format, 76, 208, 32);
		pixelColors[0x2B] = SDL_MapRGB(format, 56, 204, 108);
		pixelColors[0x2C] = SDL_MapRGB(format, 56, 180, 204);
		pixelColors[0x2D] = SDL_MapRGB(format, 60, 60, 60);
		pixelColors[0x2E] = SDL_MapRGB(format, 0, 0, 0);
		pixelColors[0x2F] = SDL_MapRGB(format, 0, 0, 0);

		pixelColors[0x30] = SDL_MapRGB(format, 236, 238, 236);
		pixelColors[0x31] = SDL_MapRGB(format, 168, 204, 236);
		pixelColors[0x32] = SDL_MapRGB(format, 188, 188, 236);
		pixelColors[0x33] = SDL_MapRGB(format, 212, 178, 236);
		pixelColors[0x34] = SDL_MapRGB(format, 236, 174, 236);
		pixelColors[0x35] = SDL_MapRGB(format, 236, 174, 212);
		pixelColors[0x36] = SDL_MapRGB(format, 236, 180, 176);
		pixelColors[0x37] = SDL_MapRGB(format, 228, 196, 144);
		pixelColors[0x38] = SDL_MapRGB(format, 204, 210, 120);
		pixelColors[0x39] = SDL_MapRGB(format, 180, 222, 120);
		pixelColors[0x3A] = SDL_MapRGB(format, 168, 226, 144);
		pixelColors[0x3B] = SDL_MapRGB(format, 152, 226, 180);
		pixelColors[0x3C] = SDL_MapRGB(format, 160, 214, 228);
		pixelColors[0x3D] = SDL_MapRGB(format, 160, 162, 160);
		pixelColors[0x3E] = SDL_MapRGB(format, 0, 0, 0);
		pixelColors[0x3F] = SDL_MapRGB(format, 0, 0, 0);
	}
	bool temp = true;
	int scanline = -1;
	int PPUcycle = 0;
	bool OddFrame;
	bool enableRendering;

	int ok = 0;

	// For nametables, coarse is tile position, fine is pixel position.
	// Tiles are 32 horizontally, 30 vertically.
	// Pixels are 8x8 in each tile.
	int txCoarse;
	int tyCoarse;
	int txFine;
	int tyFine;

	int xCoarse;
	int yCoarse;
	int xFine;
	int yFine;

	void horizontalTtoV();
	void verticalTtoV();

	void incHorizontal();
	void incVertical();

	uint16_t tileAddress;
	uint16_t attributeAddress;

	byte dataBuffer;

	void clock();

	byte readPPUBus(uint16_t address);
	void writePPUBus(uint16_t address, byte value);

	int tileID;
	byte attributeData;
	byte PTx;
	byte PTy;
	byte NTS;

	byte tileData;

	byte BR;
	byte BL;
	byte TR;
	byte TL;	
	byte pixelColor;

	int colorIndex;

	int localY;
	int localX;
	int quadrant;

	byte xF;

	// Shifters
	byte nextTileID;
	byte nextTileAttribute;
	byte nextTileLow;
	byte nextTileHigh;
	uint16_t patternLow;
	uint16_t patternHigh;
	uint16_t attributeLow;
	uint16_t attributeHigh;
	byte pixel;
	byte palette;
	uint16_t scrollShift;

	byte pixelL;
	byte pixelH;
	byte paletteL;
	byte paletteH;

	void PPU::getPatternAttributeShifters();
	void PPU::shiftShifters();

	void PPU::storeX();
	void PPU::storeY();

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
};
#endif
