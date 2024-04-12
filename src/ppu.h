#pragma once
#include <cstdint>
class ppu {
private:


public:

	void handlePPURead(uint16_t ppuRegister);
	void handlePPUWrite(uint16_t ppuRegister);
};
