#pragma once
#ifndef PPU_H
#define PPU_H
#include <cstdint>
#include <SDL.h>
#include "bus.h"
typedef uint8_t byte;

class PPU : public Singleton<PPU> {
	friend class Singleton<PPU>;
private:
	SDL_Renderer* renderer;
	byte OAM[256]{};
	uint16_t startAddr{ 0 };



public:
	void InitializeRenderer(SDL_Renderer* render) {
		this->renderer = render;
	}
	Bus* bus = Bus::getInstance();

	// Transfer CHR from Mapper to PPU
	std::vector<byte> ppuCHR;
	void initializeCHRM0(const std::vector<uint8_t>& chr);

	void checkPpuBus();

	// Made 2 variables for nametable orientation to avoid confusion
	bool horizontal; 
	bool vertical;

	void mirror(bool mirror);

	void handlePPURead(uint16_t ppuRegister, byte value);
	void handlePPUWrite(uint16_t ppuRegister, byte value);

	void writeOAM(byte address, byte value);
	byte readOAM(byte address);
};
#endif
