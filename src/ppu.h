#pragma once
#ifndef PPU_H
#define PPU_H
#include <cstdint>
#include <SDL.h>
#include "bus.h"


class PPU : public Singleton<PPU> {
	friend class Singleton<PPU>;
private:

	SDL_Renderer* renderer;
	SDL_PixelFormat* format;
	byte OAM[256]{ 0 };
	uint16_t OAMstartAddr{ 0 };
	int patternTable[0x2000];

public:

	// Pass SDL components into the PPU class
	void InitializeRenderer(SDL_Renderer* render, SDL_PixelFormat* formatted) {
		this->renderer = render;
		this->format = formatted;
	}

	// To make sure no new instances of Bus are created as that would create sync problems
	Bus* bus = Bus::getInstance();

	// Initialize CHR, transfer CHR data from Mapper to PPU
	std::vector<byte> ppuCHR;
	void initializeCHRM0(const std::vector<uint8_t>& chr); // Might change this to vector since it should be 8KB, not yet.
	void getPatternTable(byte address);
	int getPixelValue(bool PT, uint16_t tile_index, byte x, byte y);

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
		bool V; // 7: Generate NMI at start of vertical blanking (0: off, 1: on) 
		bool P; // 6: PPU master/slave select (0: read backdrop from pins, 1: output color on pins)
		bool H; // 5: Sprite size (0: 8x8 pixels, 1: 8x16 pixels)
		bool B; // 4: Background pattern table address (0: $0000, 1: $1000)
		bool S; // 3: Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode)
		bool I; // 2: VRAM address increment per CPU read/write of PPUDATA (0: add 1, going across; 1: add 32, going down)
		byte NN : 2; // 1 | 0: Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
	};
	PPUCTRL PPUCTRL;
	uint16_t NametableAddr;

	// Flags for PPUMASK
	struct PPUMASK {
		bool Blue; // 7: Emphasize blue
		bool Green; // 6: Emphasize green
		bool Red; // 5: Emphasize red
		bool s; // 4: Show sprites
		bool b; // 3: Show background
		bool M; // 2: (1: Show sprites in leftmost 8 pixels of the screen, 0: Hide)
		bool m; // 1: (1: Show background in leftmost 8 pixels of screen, 0: Hide)
		bool Greyscale; // 0: Greyscale (0: normal color, 1: produce a greyscale display)
	};
	PPUMASK PPUMASK;

	// Flags for PPUSTATUS
	struct PPUSTATUS {
		bool V; // Vertical blank has started
		bool S; // Sprite 0 hit
		bool O; // Sprite overflow but its buggy in the NES
	};
	PPUSTATUS PPUSTATUS;

	struct PPUADDR {
		byte lower;
		byte upper;
	};
	PPUADDR PPUADDR;
	uint16_t VRAMaddress;

	byte OAMAddr = 0;

	// PPU internal registers
	struct PPUREGISTERS {
		bool w; // SCROLL and ADDR share this register for writing
		// byte v;
		uint16_t t;
		byte x;

	};
	PPUREGISTERS registers;

	// Handle read/write to PPU Registers
	byte handlePPURead(uint16_t ppuRegister, byte value);
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

	int scanline = 0;

	void clock();

	byte readPPUBus(uint16_t address);
	void writePPUBus(uint16_t address, byte value);
};
#endif
