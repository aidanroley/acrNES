#pragma once
#include <cstdint>
#include "ppuBus.h"
#include "bus.h"

typedef uint8_t byte;

// class Bus;

class PPU : public Singleton<PPU> {
	friend class Singleton<PPU>;
private:
	byte OAM[256]{};
	uint16_t startAddr{ 0 };

public:
	Bus* bus = Bus::getInstance();
	ppuBus* ppuBus = ppuBus::getInstance();

	void handlePPURead(uint16_t ppuRegister, byte value);
	void handlePPUWrite(uint16_t ppuRegister, byte value);

	void writeOAM(byte address, byte value);
	byte readOAM(byte address);
};
