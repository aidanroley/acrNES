#include "PPU.h"
#include "cpu.h"
#include <SDL.h>

void PPU::mirror(bool mirror) {
	if (mirror) {
		vertical = true;
		std::cout << "horizontal" << std::endl;
	}
	else {
		horizontal = true;
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
	byte lPlane = readPPUBus(baseAddress + y); // ppuVRAM[baseAddress + y];
	byte uPlane = readPPUBus(baseAddress + y + 8); // ppuVRAM[baseAddress + y + 8];

	// Get column of the bit corresponding to pixel (x)
	byte bit0 = (lPlane >> (7 - x)) & 1;
	byte bit1 = (uPlane >> (7 - x)) & 1;

	// Concatenate bits
	byte pixelOffset = (bit1 << 1) | bit0;
	return pixelOffset;

}

void PPU::horizontalTtoV() {
	if (PPUMASK.b) {
		VRAMaddress = (VRAMaddress & 0x7BE0) | (registers.t & 0x041F);
	}

}

void PPU:: verticalTtoV() {
	if (PPUMASK.b) {
		VRAMaddress = (VRAMaddress & 0x041F) | (registers.t & 0x7BE0);
	}
	

}

void PPU::incHorizontal() {
	if (PPUMASK.b) {
		if ((VRAMaddress & 0x001F) == 31) {
			VRAMaddress &= ~0x001F;
			VRAMaddress ^= 0x0400;
		}
		else {
			VRAMaddress++;
		}
	}

}

void PPU::incVertical() {
	if (PPUMASK.b) {
		// std::cout << "VRAMADDRESS" << VRAMaddress << std::endl;
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

}

byte PPU::handlePPURead(uint16_t ppuRegister) {
	//std::cout << "oops" << std::endl;
	//std::cout << ppuRegister << std::endl;
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
		//std::cout << "what" << std::endl;
		// Add here later maybe
		if (PPUSTATUS.V) {
			// std::cout << "GOOD!" << std::endl;
			data |= (1 << 7);  // Set bit 7

		}
		if (PPUSTATUS.S) {
			data |= (1 << 6);  // Set bit 6
		}
		if (PPUSTATUS.O) {
			data |= (1 << 5);  // Set bit 5
		}
		PPUSTATUS.V = 0;
		registers.w = 0; // fix this later
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
		// std::cout << "THIS CANNOT BE OVER 3FFF" << std::hex << VRAMaddress << std::endl;
		data = dataBuffer;
		dataBuffer = readPPUBus(VRAMaddress); //ppuVRAM[VRAMaddress];

		// Buffer not needed for palette memory
		if (VRAMaddress > 0x3EFF) {
			data = readPPUBus(VRAMaddress); // ppuVRAM[VRAMaddress];
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
		registers.t = (registers.t & ~(0x03 << 10)) | (PPUCTRL.NN << 10);

		//std::cout << "PPU CTRL VALUE " << std::hex << static_cast<int>(value) << std::endl;
		//std::cout << "NMI ON VbLANK ? " << PPUCTRL.V << std::endl;
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

		//std::cout << "PPU MASK VALUE " << std::hex << static_cast<int>(value) << std::endl;
		//std::cout << "BG ENABLED? " << PPUMASK.b << std::endl;

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
			/*
			std::cout << "PPUCYCLE: " << std::dec << static_cast<int>(bus->ppuCycles) << std::hex << std::endl;
			std::cout << "UPPER: " << static_cast<int>(value) << std::endl; 
			*/
			PPUADDR.upper = value & 0x3F;
			registers.w = 1;
		}
		else {
			//std::cout << "PPUCYCLE: " << std::dec << static_cast<int>(bus->ppuCycles) << std::hex << std::endl;
			//std::cout << "LOWER: " << static_cast<int>(value) << std::endl;
			PPUADDR.lower = value;
			//std::cout << "CONCATENATED: " << static_cast<int>(((PPUADDR.upper << 8) | PPUADDR.lower)) << std::endl;
			VRAMaddress = (((PPUADDR.upper << 8) & 0xFF00) | PPUADDR.lower);
			registers.w = 0;
		}
		break;
		
		// PPUDATA
	case 0x2007:
	/*
			std::cout << "2007 CALLED VALUE: " << std::hex << static_cast<int>(value) << " ADDRess: " << std::hex << VRAMaddress << std::endl;
			std::cout << "PPUcycleTotal" << std::dec << static_cast<int>(bus->ppuCycles) << std::endl;
			std::cout << "ppucycle/scanline" << std::dec << static_cast<int>(PPUcycle) << " / " << static_cast<int>(scanline) << std::endl;
			*/
			
		
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

byte PPU::readPPUBus(uint16_t address) {
	if (address >= 0x3F20 && address <= 0x3FFF) {
		address = (address & 0x001F) | 0x3F00;

	}
	else if (address == 0x3F10) {
		address == 0x3F00;
	}
	else if (address == 0x3F14) {
		address == 0x3F04;
	}
	else if (address == 0x3F18) {
		address == 0x3F08;
	}
	else if (address == 0x3F1C) {
		address == 0x3F0C;
	}
	return ppuVRAM[address];
}

void PPU::getPatternAttributeShifters() {
	patternLow = (patternLow & 0xFF00) | nextTileLow;
	if (PPUMASK.b) {
		//std::cout << "patternLow: " << std::hex << static_cast<int>(patternLow) << std::endl;
	}
	patternHigh = (patternHigh & 0xFF00) | nextTileHigh;
	attributeLow = (attributeLow & 0xFF00) | ((nextTileAttribute & 0b01) ? 0xFF : 0x00);
	attributeHigh = (attributeHigh & 0xFF00) | ((nextTileAttribute & 0b10) ? 0xFF : 0x00);
}

void PPU::shiftShifters() {
	if (PPUMASK.b) {
		patternLow <<= 1;
		patternHigh <<= 1;
		attributeLow <<= 1;
		attributeHigh <<= 1;
	}
}

void PPU::storeY() {
	if (PPUMASK.b || PPUMASK.s) {
		VRAMaddress &= ~0x0C00;
		VRAMaddress |= (registers.t & 0x0C00);

		VRAMaddress &= ~0x03E0;
		VRAMaddress |= (registers.t & 0x03E0);

		VRAMaddress &= ~0xF000;
		VRAMaddress |= (registers.t & 0xF000);
	}
}

void PPU::storeX() {
	if (PPUMASK.b || PPUMASK.s) {
		VRAMaddress &= ~0x0C00;
		VRAMaddress |= (registers.t & 0x0C00);

		VRAMaddress &= ~0x001F;
		VRAMaddress |= (registers.t & 0x001F);
	}

}
void PPU::clock() {
	if (VRAMaddress > 0x1E) {
		//std::cout << "BAD" << std::dec << PPUcycle << std::hex << VRAMaddress << std::endl;
	}


	if (scanline > -1 && scanline < 240 && PPUcycle == 257 && PPUMASK.b) {
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

		else if (scanline == -1 && PPUcycle >= 280 && PPUcycle <= 304 && PPUMASK.b) {

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
			shiftShifters();
			
			switch (PPUcycle % 8) {
					
			case 0:
				if (PPUcycle == 256) {	
					// incHorizontal();

					if (PPUMASK.b || PPUMASK.s) {
						incVertical();
					}
				}
				/*
				localX = PTx % 4;
				localY = PTy % 4;

				if (localY < 2) {
					quadrant = (localX < 2) ? TL : TR;
				}
				else {
					quadrant = (localY < 2) ? BL : BR;
				}
				tileData = getPixelValue(NTS, tileID, PTx, PTy); 

				colorIndex = readPPUBus(0x3F00 + quadrant * 4 + tileData);// ppuVRAM[0x3F00 + quadrant * 4 + tileData];
				/*
				SDL_SetRenderDrawColor(renderer,
					(pixelColors[0x00] >> 16) & 0xFF, // Red component
					(pixelColors[0x00] >> 8) & 0xFF,  // Green component
					pixelColors[0x00] & 0xFF,         // Blue component
					SDL_ALPHA_OPAQUE);                // Full opacity

				SDL_RenderDrawPoint(renderer, xCoarse * 8 + xFine, yCoarse * 8 + yFine);
				SDL_RenderPresent(renderer); // Update screen with the rendering performed
				
				// std::cout << std::hex << "Color index" << colorIndex << std::endl;

				xF = VRAMaddress & 0x07;

				SetPixel((VRAMaddress & 0x1F) * 8 + xFine, yCoarse * 8 + ((VRAMaddress >> 12) & 0x07), pixelColors[colorIndex]);
				if (PPUMASK.b) {
					std::cout << "VRAM ADDR" << std::hex << static_cast<int>(VRAMaddress) << std::endl;
					std::cout << "so close" << std::dec << static_cast<int>((VRAMaddress & 0x1F) * 8 + xF) << "XF" << static_cast<int>(xF) << std::hex << std::endl;
					
				}
				
				if (printDebug && tileID != 0) {
					ok++;
					std::cout << "VRAM ADDR " << (VRAMaddress & 0x0FFF) << " COUNT " << std::dec << ok << "CYCLE " << PPUcycle << std::hex << std::endl;
					std::cout << "x coarse" << ((VRAMaddress) & 0x1F) << std::endl;
					std::cout << "y coarse" << (yCoarse) << std::endl;
					std::cout << "COLOR INDEx " << colorIndex << std::endl;
					std::cout << "TILE ADDRESS: " << tileAddress << std::endl;
					std::cout << "TILE ID: " << tileID << std::endl;
				}
				
				
				*/

				incHorizontal();
				break;

			case 1:
				getPatternAttributeShifters();
				break;

			case 2:
				tileAddress = 0x2000 | (VRAMaddress & 0x0FFF);
				//std::cout << "tile address" << std::hex << tileAddress << std::endl;
				//std::cout << "tile ID" << tileID << std::endl;
				nextTileID = readPPUBus(tileAddress); // ppuVRAM[tileAddress];
				if (PPUMASK.b) {
					//std::cout << "nextTileID: " << std::hex << static_cast<int>(nextTileID) << std::endl;
				}
				break;

			case 3:
				// nextTileAttribute = 
				break;

			case 4:
				attributeAddress = 0x23C0 | (VRAMaddress & 0x0C00) | ((VRAMaddress >> 4) & 0x38) | ((VRAMaddress >> 2) & 0x07);
				nextTileAttribute = readPPUBus(attributeAddress);
				// std::cout << "nextTileAttributeOG: " << std::hex << static_cast<int>(nextTileAttribute) << std::endl;

				if ((VRAMaddress >> 5) & 0x02) {
					nextTileAttribute >>= 4;
				}
				if (VRAMaddress & 0x02) {
					nextTileAttribute >>= 2;
				}
				nextTileAttribute &= 0x03;
				if (PPUMASK.b) {
					// std::cout << "Attributeaddres: " << std::hex << static_cast<int>(attributeAddress) << std::endl;
					// std::cout << "nextTileAttribute: " << std::hex << static_cast<int>(nextTileAttribute) << std::endl;
				}

				/*
				if (PPUMASK.b) {
					std::cout << "VRAM ADDR " << std::hex << static_cast<int>(VRAMaddress) << std::endl;
					std::cout << "attribute address " << std::hex << static_cast<int>(attributeAddress) << std::endl;
				}
				PTx = (attributeAddress >> 2) & 0x07;
				PTy = (attributeAddress >> 4) & 0x07;
				NTS = (attributeAddress >> 10) & 0x03;
				if (PPUMASK.b) {
					std::cout << "PTx: " << std::hex << static_cast<int>(PTx) << std::endl;
					std::cout << "PTy: " << std::hex << static_cast<int>(PTy) << std::endl;
					std::cout << "NTS: " << std::hex << static_cast<int>(NTS) << std::endl;
				}
				attributeData = readPPUBus(attributeAddress); // ppuVRAM[attributeAddress];
				//std::cout << "attribute data " << std::hex << static_cast<int>(attributeData) << std::endl;
				BR = attributeData >> 6;
				BL = attributeData >> 4 & 0x03;
				TR = attributeData >> 2 & 0x03;
				TL = attributeData >> 0 & 0x03;
				attributeData = BR << 6 | BL << 4 | TR << 2 | TL;
				*/
				
				break;

			case 5:
				nextTileLow = readPPUBus((PPUCTRL.B << 12) + ((uint16_t)nextTileID << 4) + (VRAMaddress >> 12));
				if (PPUMASK.b) {
					//std::cout << "nextTileLow: " << std::hex << static_cast<int>(nextTileLow) << std::endl;
				}
				break;

			case 6:
				nextTileHigh = readPPUBus((PPUCTRL.B << 12) + ((uint16_t)nextTileID << 4) + (VRAMaddress >> 12) + 8);
				if (PPUMASK.b) {
					//std::cout << "nextTileHigh: " << std::hex << static_cast<int>(nextTileHigh) << std::endl;
				}
				// tileDataLow = getPixelValue(NTS, tileID, PTx, PTy);
				break;
				
			case 7:
				break;

			}

		}
		else if (PPUcycle > 256 && PPUcycle < 321 && scanline < 240) {

		}

		if (PPUcycle == 257) {
			getPatternAttributeShifters();
			storeX();
		}

		if (PPUcycle == 338 || PPUcycle == 340) {
			nextTileID = readPPUBus(0x2000 | (VRAMaddress & 0xFF00));
		}

		if (scanline == -1 && PPUcycle >= 280 && PPUcycle < 305) {
			storeY();
		}

	}

	// Vertical blank has started
	if (scanline == 241 && PPUcycle == 1) {

		PPUSTATUS.V = true;
		if (PPUCTRL.V) {
			bus->transferNMI();
		}

	}

	if (PPUMASK.b) {

		if (PPUMASK.m || PPUcycle > 8) {

			scrollShift = 0x8000 >> xFine;
			pixelL = (patternLow & scrollShift) > 0;
			pixelH = (patternHigh & scrollShift) > 0;

			pixel = pixelH << 1 | pixelL;

			paletteL = (attributeLow & scrollShift) > 0;
			paletteH = (attributeHigh & scrollShift) > 0;

			palette = paletteH << 1 | paletteL;
			if (PPUMASK.b && patternLow != 0) {
				//std::cout << "final pixel: " << std::hex << static_cast<int>(pixel) << std::endl;
			}
		}
	}

		uint16_t finalAddr = readPPUBus(0x3F00 + pixel + 4 * palette); //(0x3F00 | ((palette << 2) | pixel));

	if (palette != 0 && palette != 3) {
		//std::cout << "palette " << std::hex << static_cast<int>(palette) << std::endl << "finalAddr" << static_cast<int>(finalAddr) << std::endl;
	}
	if (pixel != 0 && temp) {
			temp = false;
		//std::cout << std::dec << "pixel " << static_cast<int>(pixel) << std::hex << std::endl;
		for (int addr = 0x3F00; addr <= 0x3FFF; addr++) {
			//std::cout << "Address " << std::hex << std::setw(4) << std::setfill('0') << addr
				//<< ": " << std::setw(2) << static_cast<int>(ppuVRAM[addr]) << std::endl;
		}
		
				
	}

	if (PPUMASK.b && patternLow != 0) {
		
		//std::cout << "FINAL ADDRESS: " << std::hex << static_cast<int>(0x3F00 + (((palette << 2) + pixel) & 0x3F)) << std::endl;
		//std::cout << "pixel: " << std::hex << static_cast<int>(finalAddr) << std::endl;
		//std::cout << "palette: " << std::hex << static_cast<int>(finalAddr) << std::endl;

		//std::cout << "PPU CYCLE: " << std::dec << bus->ppuCycles << std::endl;
		
	}
	SetPixel(PPUcycle - 1, scanline, pixelColors[finalAddr]);

	if (PPUcycle == 340 && scanline != 260 ) {
		if (scanline == 0) {
			// std::cout << "PPU checkpoint" << scanline << std::endl;
		}
		PPUcycle = -1; // This is -1 because PPUcycle is incremented directly after this
		scanline++;
	}

	if (scanline == 260 && PPUcycle == 340) {
		UpdateScreen();
		// std::cout << "screen updated" << std::endl;
		PPUcycle = 0;
		scanline = -1;
		printDebug = false;
	}

	PPUcycle++;
}

