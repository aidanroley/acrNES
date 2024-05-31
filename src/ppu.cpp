#include "PPU.h"
#include "cpu.h"
#include <SDL.h>

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

// 16x16 grid 4x4 pixel tiles
// Each bit in first plane controls bit 0, corresponding bit in second plane controls bit 1
// 0x0000 - 0x1FFF (PATTERN)
// 0x3F00 - 0x3FFF (PALETTE)

void PPU::getPatternTable(byte address) {
	byte BG = ppuVRAM[0x3F00];

}

// You need to know the tile number, which pattern table, and row/column to get the pixel value
// Here the Y is the row, X is the column within the tile specified
// PT is which pattern table it's in

int PPU::getPixelValue(byte PT, uint16_t tile_index, byte x, byte y) {

	// Fetch base address of the tile
	uint16_t baseAddress = tile_index * 16 + PT; //(PT ? 0x1000 : 0);

	// Get upper and lower plane address
	byte lPlane = ppuVRAM[baseAddress + y];
	byte uPlane = ppuVRAM[baseAddress + y + 8];

	// Get column of the bit corresponding to pixel (x)
	byte bit0 = (lPlane >> (7 - x)) & 1;
	byte bit1 = (uPlane >> (7 - x)) & 1;

	// Concatenate bits
	byte pixelOffset = (bit1 << 1) | bit0;
	return pixelOffset;

}

void PPU::horizontalTtoV() {
	 VRAMaddress = (VRAMaddress & 0x7BE0) | (registers.t & 0x041F);

}

void PPU:: verticalTtoV() {
	VRAMaddress = (VRAMaddress & 0x041F) | (registers.t & 0x7BE0);
	

}

void PPU::incHorizontal() {
	if ((VRAMaddress & 0x001F) == 31) {
		VRAMaddress &= ~0x001F;
		VRAMaddress ^= 0x0400;
	}
	else {
		VRAMaddress++;
	}

}

void PPU::incVertical() {
	if ((VRAMaddress & 0x7000) != 0x7000) {
		VRAMaddress += 0x1000;
	}
	else {
		VRAMaddress &= ~0x7000;
		yCoarse = (VRAMaddress & 0x03E0) >> 5;
		if (yCoarse == 29) {
			yCoarse = 0;
			VRAMaddress ^= 0x8000;
		}
		else if (yCoarse == 31) {
			yCoarse = 0;
		}
		else {
			yCoarse += 1;
		}
		VRAMaddress = (VRAMaddress & ~0x03E0) | (yCoarse << 5);
	}

}

byte PPU::handlePPURead(uint16_t ppuRegister) {
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
		data = dataBuffer;
		dataBuffer = ppuVRAM[VRAMaddress];

		// Buffer not needed for palette memory
		if (VRAMaddress > 0x3EFF) {
			data = ppuVRAM[VRAMaddress];
		}

		VRAMaddress += (PPUCTRL.I == 0 ? 1 : 32);
		// VRAMaddress & 0x3FFF;
		break;

	}
	return data;
	
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
		case 0: TempNametableAddr = 0x2000; break;
		case 1: TempNametableAddr = 0x2400; break;
		case 2: TempNametableAddr = 0x2800; break;
		case 3: TempNametableAddr = 0x2C00; break;
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
		OAMAddr = (OAMAddr + 1) % 256;
		break;

		// PPUSCROLL
	case 0x2005:
		if (registers.w == 0) {
			// X scroll
			xFine = value & 0x07; // Set this to lower 3 bits
			txCoarse = value >> 3; // Upper 5 bits
			registers.t = (registers.t & 0x7FE0) | (txCoarse);
			registers.w = 1;
		}
		else {
			// Y scroll
			tyFine = value & 0x07;
			tyCoarse = value >> 3;
			registers.t = (registers.t & 0x0C1F) | ((tyFine) << 12);
			registers.t = (registers.t & 0xF83F) | ((tyCoarse) << 5);
			registers.w = 0;
		}
		break;

		// PPUADDR
	case 0x2006:
		if (registers.w == 0) {
			PPUADDR.upper = value;
			registers.w = 1;
		}
		else {
			PPUADDR.lower = value;
			VRAMaddress = ((PPUADDR.upper << 8) | PPUADDR.lower) & 0x3FFF;
			registers.w = 0;
		}
		break;

		// PPUDATA
	case 0x2007:
		writePPUBus(VRAMaddress, value); // ppuVRAM[VRAMaddress] = value;
		VRAMaddress += (PPUCTRL.I == 0 ? 1 : 32);
		VRAMaddress & 0x3FFF;
		break;

		// OAMDMA
	case 0x4014:
		OAMstartAddr = value * 0x100;
		bus->dmaTransfer(OAMstartAddr, OAM, 256);

		// Stall CPU 512 cycles here
		break;

	}


}

void PPU::writeOAM(byte address, byte value) {

}

void PPU::checkPpuBus() {
	std::cout << "CHR PLEASE WORK!!!!" << std::endl;
	for (const auto& value : ppuVRAM) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(value) << " ";
	}
	pixelBuffer.resize(256 * 240);

	 //SDL_RenderPresent(renderer);	
}


// Because of how nametables are set up, writing to one address may affect another address
void PPU::writePPUBus(uint16_t address, byte value) {

	// Pattern memory
	if (address >= 0x0000 && address < 0x2000) {
		ppuVRAM[address] = value;
	}

	// This is the nametable range in memory
	else if (address >= 0x2000 && address <= 0x3EFF) {

		// If nametables are horiontally mirrored
		if (horizontal) {

			if (address < 0x2400) {
				ppuVRAM[address] = value;
				ppuVRAM[address + 0x400] = value; // Mirror to that nametable

			}
			else if (address < 0x2800) {
				ppuVRAM[address] = value;
				ppuVRAM[address - 0x400] = value;
			}
			else if (address < 0x2C00) {
				ppuVRAM[address] = value;
				ppuVRAM[address + 0x400] = value;
			}
			else {
				ppuVRAM[address] = value;
				ppuVRAM[address - 0x400] = value;
			}
		}

		// If nametables are vertically mirrored
		if (vertical) {
			
			if (address < 0x2400) {
				ppuVRAM[address] = value;
				ppuVRAM[address + 0x800] = value;
			}
			else if (address < 0x2800) {
				ppuVRAM[address] = value;
				ppuVRAM[address + 0x800] = value;
			}
			else if (address < 0x2C00) {
				ppuVRAM[address] = value;
				ppuVRAM[address - 0x800] = value;
			}
			else {
				ppuVRAM[address] = value;
				ppuVRAM[address - 0x800] = value;
			}
		}
		
	}

	// This handles palette memory mirrors
	else if (address >= 0x3F00 && address <= 0x3FFF) {

		uint16_t mirroredPaletteAddr = 0;

		// Everything at and above 0x3F20 are mirorrs of addresses at and below 0x3F1F
		if (address >= 0x3F20) {
			address = (address & 0x001F) | 0x3F00;
		}

		if (address == 0x3F10) {
			mirroredPaletteAddr = 0x3F00;
		}
		else if (address == 0x3F14) {
			mirroredPaletteAddr = 0x3F04;
		}
		else if (address == 0x3F18) {
			mirroredPaletteAddr = 0x3F08;
		}
		else if (address == 0x3F1C) {
			mirroredPaletteAddr = 0x3F0C;
		}

		ppuVRAM[address] = value;

		// If mirroredPalletteAddr has been altered then write to its address
		if (mirroredPaletteAddr != 0) {
			ppuVRAM[mirroredPaletteAddr] = value;
		}
	}

}

void PPU::clock() {


	if (scanline > -1 && scanline < 240 && PPUcycle == 257) {
		//SetPixel(100, 100, 0xFFFF0000);
		// SDL_RenderDrawPoint(renderer, 230, 200);
		//UpdateScreen();
		//std::cout << "updated screen" << std::endl;

		horizontalTtoV();

	}

	// https://www.nesdev.org/w/images/default/4/4f/Ppu.svg <-- Very useful diagram
	if (scanline < 240) {

		if (scanline == -1 && PPUcycle == 1) {

			// Clear sprite 0, overflow, vblank
			PPUSTATUS.O = false;
			PPUSTATUS.V = false;
			PPUSTATUS.S = false;
		}

		else if (scanline == -1 && PPUcycle >= 280 && PPUcycle <= 304) {

			verticalTtoV();
			
		}


		else if (scanline == 0 && PPUcycle == 0 && OddFrame && PPUMASK.b) {

			PPUcycle = 1;
		}

		else if (scanline > 0 && PPUcycle == 0) {

			// Idle
		}

		// At 256 it increments vert(v) as well
		else if (PPUcycle > 0 && (PPUcycle <= 256 || PPUcycle >= 321) && scanline < 240) {
			
			switch (PPUcycle % 8) {
					
			case 0:
				if (PPUcycle == 256) {
					incHorizontal();

					if (PPUMASK.b || PPUMASK.s) {
						incVertical();
					}
				}
				localX = PTx % 4;
				localY = PTy % 4;
				quadrant;
				if (localY < 2) {
					quadrant = (localX < 2) ? TL : TR;
				}
				else {
					quadrant = (localY < 2) ? BL : BR;
				}
				tileData = getPixelValue(NTS, tileID, PTx, PTy); 

				colorIndex = ppuVRAM[0x3F00 + quadrant * 4 + tileData];
				/*
				SDL_SetRenderDrawColor(renderer,
					(pixelColors[0x00] >> 16) & 0xFF, // Red component
					(pixelColors[0x00] >> 8) & 0xFF,  // Green component
					pixelColors[0x00] & 0xFF,         // Blue component
					SDL_ALPHA_OPAQUE);                // Full opacity

				SDL_RenderDrawPoint(renderer, xCoarse * 8 + xFine, yCoarse * 8 + yFine);
				SDL_RenderPresent(renderer); // Update screen with the rendering performed
				*/
				// std::cout << std::hex << "Color index" << colorIndex << std::endl;
				SetPixel(xCoarse * 8 + xFine, yCoarse * 8 + yFine, pixelColors[colorIndex]);



				incHorizontal();
				break;

			case 1:
				break;

			case 2:
				tileAddress = 0x2000 | (VRAMaddress & 0x0FFF);
				tileID = ppuVRAM[tileAddress];
				break;

			case 3:
				break;

			case 4:
				attributeAddress = 0x23C0 | (VRAMaddress & 0x0C00) | ((VRAMaddress >> 4) & 0x38) | ((VRAMaddress >> 2) & 0x07);
				PTx = (attributeAddress >> 2) & 0x07;
				PTy = (attributeAddress >> 4) & 0x07;
				NTS = (attributeAddress >> 10) & 0x03;
				attributeData = ppuVRAM[attributeAddress];
				BR = attributeData >> 6;
				BL = attributeData >> 4 & 0x03;
				TR = attributeData >> 2 & 0x03;
				TL = attributeData >> 0 & 0x03;
				attributeData = BR << 6 | BL << 4 | TR << 2 | TL;
				break;

			case 5:
				break;

			case 6:
				// tileDataLow = getPixelValue(NTS, tileID, PTx, PTy);
				break;

			case 7:
				break;

			}

		}
		else if (PPUcycle > 256 && PPUcycle < 321 && scanline < 240) {

		}



	}

	// Vertical blank has started
	if (scanline == 241 && PPUcycle == 1) {

		PPUSTATUS.V == true;

	}

	else if (PPUcycle == 340 && scanline != 260 ) {
		if (scanline == 0) {
			// std::cout << "PPU checkpoint" << scanline << std::endl;
		}
		PPUcycle = -1; // This is -1 because PPUcycle is incremented directly after this
		scanline++;
	}

	else if (scanline == 260 && PPUcycle == 340) {
		UpdateScreen();
		// std::cout << "screen updated" << std::endl;
		PPUcycle = -1;
		scanline = -1;
	}

	PPUcycle++;
}

