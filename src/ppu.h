#pragma once
#include <cstdint>
#include "ppuBus.h"
#include "bus.h"

typedef uint8_t byte;

class Bus;

class PPU {
private:
	Bus* bus;
	byte OAM[256];

public:
	PPU(Bus* bus);

	uint16_t startAddr;
	void handlePPURead(uint16_t ppuRegister, byte value);
	void handlePPUWrite(uint16_t ppuRegister, byte value);

	void writeOAM(byte address, byte value);
	byte readOAM(byte address);
};
