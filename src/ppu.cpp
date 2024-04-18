#include "PPU.h"
#include "cpu.h"

void PPU::mirror(bool mirror) {
	if (mirror) {
		horizontal = true;
		std::cout << "horizontal" << std::endl;
	}
	else {
		vertical = true;
		std::cout << "vertical" << std::endl;
	}


}

void PPU::handlePPURead(uint16_t ppuRegister, byte value) {
	byte data = 0;
	switch (ppuRegister) {

	// PPUCTRL
	case 0x2000:
		break; 

	// PPUMASK
	case 0x2001:
		break;

	// PPUSTATUS
	case 0x2002:
		// Add here later maybe
		PPUSTATUS.V = 0;
		break;

	// OAMADDR
	case 0x2003:
		break;

	// OAMDATA
	case 0x2004:
		data = OAM[OAMAddr];
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
		PPUCTRL.V = (value >> 7) & 1;
		PPUCTRL.P = (value >> 6) & 1;
		PPUCTRL.H = (value >> 5) & 1;
		PPUCTRL.B = (value >> 4) & 1;
		PPUCTRL.S = (value >> 3) & 1;
		PPUCTRL.I = (value >> 2) & 1;
		PPUCTRL.NN = value & 0x03;

		// Change nametable address based on NN flag
		switch (PPUCTRL.NN) {
		case 0: NametableAddr = 0x2000; break;
		case 1: NametableAddr = 0x2400; break;
		case 2: NametableAddr = 0x2800; break;
		case 3: NametableAddr = 0x2C00; break;
		}
		break;
	
		// PPUMASK
	case 0x2001:
		PPUMASK.Blue = (value >> 7) & 1;
		PPUMASK.Green = (value >> 6) & 1;
		PPUMASK.Red = (value >> 5) & 1;
		PPUMASK.s = (value >> 4) & 1;
		PPUMASK.b = (value >> 3) & 1;
		PPUMASK.M = (value >> 2) & 1;
		PPUMASK.m = (value >> 1) & 1;
		PPUMASK.Greyscale = (value >> 0) & 1;

		break;

		// PPUSTATUS
	case 0x2002:
		break;

		// OAMADDR
	case 0x2003:
		OAMAddr = value;
		break;

		// OAMDATA
	case 0x2004:
		OAM[OAMAddr] = value;
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

void PPU::checkPpuBus() {
	std::cout << "CHR PLEASE WORK!!!!" << std::endl;
	for (const auto& value : ppuCHR) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(value) << " ";
	}
	SDL_RenderDrawPoint(renderer, 230, 200);
	SDL_RenderPresent(renderer);	
}