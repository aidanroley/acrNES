#include "ppu.h"
#include "bus.h"

PPU::PPU(Bus* bus) : bus(bus) {}
void PPU::handlePPURead(uint16_t ppuRegister, byte value) {
	switch (ppuRegister) {

	// PPUCTRL
	case 0x2000:
		break;

	// PPUMASK
	case 0x2001:
		break;

	// PPUSTATUS
	case 0x2002:
		break;

	// OAMADDR
	case 0x2003:
		break;

	// OAMDATA
	case 0x2004:
		break;

	// PPUSCROLL
	case 0x2005:
		break;

	// PPUADDR
	case 0x2006:
		break;

	// PPUDATA
	case 0x2007:
		break;

	}
	
}
void PPU::handlePPUWrite(uint16_t ppuRegister, byte value) {
	switch (ppuRegister) {

		// PPUCTRL
	case 0x2000:
		break;

		// PPUMASK
	case 0x2001:
		break;

		// PPUSTATUS
	case 0x2002:
		break;

		// OAMADDR
	case 0x2003:
		break;

		// OAMDATA
	case 0x2004:
		break;

		// PPUSCROLL
	case 0x2005:
		break;

		// PPUADDR
	case 0x2006:
		break;

		// PPUDATA
	case 0x2007:
		break;

		// OAMDMA
	case 0x4014:
		startAddr = value * 0x100;
		bus->dmaTransfer(startAddr, OAM, 256);

		// Stall CPU 512 cycles here
		break;

	}


}

void PPU::writeOAM(byte address, byte value) {

}