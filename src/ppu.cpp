#include "PPU.h"
#include "cpu.h"
#include <SDL.h>

void PPU::mirror(bool mirror) {
	if (mirror) {
		vertical = true;
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
	uint16_t baseAddress = tile_index * 16 + PT; 

	// Get upper and lower plane address
	byte lPlane = readPPUBus(baseAddress + y); 
	byte uPlane = readPPUBus(baseAddress + y + 8); 

	// Get column of the bit corresponding to pixel (x)
	byte bit0 = (lPlane >> (7 - x)) & 1;
	byte bit1 = (uPlane >> (7 - x)) & 1;

	// Concatenate bits
	byte pixelOffset = (bit1 << 1) | bit0;
	return pixelOffset;

}

void PPU::horizontalTtoV() {
	if (PPUMASK.b || PPUMASK.s) {
		VRAMaddress = (VRAMaddress & 0x7BE0) | (registers.t & 0x041F);
	}

}

void PPU:: verticalTtoV() {
	if (PPUMASK.b || PPUMASK.s) {
		VRAMaddress = (VRAMaddress & 0x041F) | (registers.t & 0x7BE0);
	}
	

}

void PPU::incHorizontal() {
	if (PPUMASK.b || PPUMASK.s) {
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
	if (PPUMASK.b || PPUMASK.s) {

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

uint8_t PPU::flipBits(uint8_t tempByte) {

    tempByte = ((tempByte & 0xF0) >> 4) | ((tempByte & 0x0F) << 4); // Swap nibbles
	tempByte = ((tempByte & 0xCC) >> 2) | ((tempByte & 0x33) << 2); // Swap pairs
	tempByte = ((tempByte & 0xAA) >> 1) | ((tempByte & 0x55) << 1); // Swap individual bits
    return tempByte;
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

		if (PPUSTATUS.V) {
			data |= (1 << 7);  // Set bit 7

		}
		if (PPUSTATUS.S) {
			data |= (1 << 6);  // Set bit 6
		}
		if (PPUSTATUS.O) {
			data |= (1 << 5);  // Set bit 5
		}
		PPUSTATUS.V = 0;
		registers.w = 0; 
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
		dataBuffer = readPPUBus(VRAMaddress); //ppuVRAM[VRAMaddress];

		// Buffer not needed for palette memory
		if (VRAMaddress > 0x3EFF) {
			data = readPPUBus(VRAMaddress); // ppuVRAM[VRAMaddress];
		}

		VRAMaddress += (PPUCTRL.I == 0 ? 1 : 32);
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
			PPUADDR.upper = value & 0x3F;
			registers.w = 1;
		}
		else {
			PPUADDR.lower = value;
			VRAMaddress = (((PPUADDR.upper << 8) & 0xFF00) | PPUADDR.lower);
			registers.w = 0;
		}
		break;
		
		// PPUDATA
	case 0x2007:

		writePPUBus(VRAMaddress, value); // ppuVRAM[VRAMaddress] = value;
		VRAMaddress += (PPUCTRL.I == 0 ? 1 : 32);
		VRAMaddress & 0x3FFF;
		break;

	}


}

void PPU::writeOAM(byte address, byte value) {

}

void PPU::checkPpuBus() {

	pixelBuffer.resize(256 * 240);
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
	
	if (PPUMASK.s && PPUcycle > 0 && PPUcycle < 258) {

		for (int i = 0; i < numSprites; i++) {

			if (sprites[i].x > 0) {

				sprites[i].x--;
			}

			else {

				lowSprites[i] <<= 1;
				highSprites[i] <<= 1;
			}
		}
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

	if (scanline > -1 && scanline < 240 && PPUcycle == 257 && PPUMASK.b) {

		horizontalTtoV();

	}

	// https://www.nesdev.org/w/images/default/4/4f/Ppu.svg <-- Very useful diagram
	if (scanline < 240) {

		if (scanline == -1 && PPUcycle == 1) {

			// Clear sprite 0, overflow, vblank
			PPUSTATUS.O = false;
			PPUSTATUS.V = false;
			PPUSTATUS.S = false;

			for (int i = 0; i < 8; i++) {

				lowSprites[i] = 0;
				highSprites[i] = 0;
			}
		}

		else if (scanline == -1 && PPUcycle >= 280 && PPUcycle <= 304 && (PPUMASK.b || PPUMASK.s)) {

			verticalTtoV();
			
		}


		else if (scanline == 0 && PPUcycle == 0 && OddFrame && (PPUMASK.b || PPUMASK.s)) {

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

					if (PPUMASK.b || PPUMASK.s) {
						incVertical();
					}
				}
				
				incHorizontal();
				break;

			case 1:
				getPatternAttributeShifters();
				break;

			case 2:
				tileAddress = 0x2000 | (VRAMaddress & 0x0FFF);
				nextTileID = readPPUBus(tileAddress); // ppuVRAM[tileAddress];
				break;

			case 3:
				break;

			case 4:
				attributeAddress = 0x23C0 | (VRAMaddress & 0x0C00) | ((VRAMaddress >> 4) & 0x38) | ((VRAMaddress >> 2) & 0x07);
				nextTileAttribute = readPPUBus(attributeAddress);

				if ((VRAMaddress >> 5) & 0x02) {
					nextTileAttribute >>= 4;
				}
				if (VRAMaddress & 0x02) {
					nextTileAttribute >>= 2;
				}
				nextTileAttribute &= 0x03;
				break;

			case 5:
				nextTileLow = readPPUBus((PPUCTRL.B << 12) + ((uint16_t)nextTileID << 4) + (VRAMaddress >> 12));
				break;

			case 6:
				nextTileHigh = readPPUBus((PPUCTRL.B << 12) + ((uint16_t)nextTileID << 4) + (VRAMaddress >> 12) + 8);
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

	// Sprites

	if (PPUcycle == 257 && scanline > -1 && scanline < 240) {

		memset(sprites, 0, sizeof(sprites)); // Clear sprites array since there is only one per scanline
		numSprites = 0; // Reset sprite count since it's per scanline

		canSpriteZero = false;

		// Clear shifters
		for (int i = 0; i < 8; i++) {
				
			lowSprites[i] = 0;
			highSprites[i] = 0;
		}

		OAMsprite = 0;

		// 64 sprites in OAM total, 8 sprites in each scanline
		while (OAMsprite < 64 && numSprites <= 8) {

			int16_t d = ((int16_t)scanline - (int16_t)OAM[OAMsprite * 4]); // Gets the y-corrdinate difference


			if (d >= 0 && d < ((PPUCTRL.H) ? 16 : 8) && numSprites < 8) {	// PPUCTRL.H sets whether height is 8 or 16
				if (numSprites < 8) {

					if (OAMsprite == 0) {

						canSpriteZero = true;
					}

					memcpy(&sprites[numSprites], &OAM[OAMsprite * 4], 4); // Copies 4 bytes from OAM to sprites
				}
				numSprites++;
			}

			OAMsprite++;
		}

		if (numSprites >= 8) {

			PPUSTATUS.O = true;
		}

	}

	if (PPUcycle == 340 && scanline < 240 && scanline > -1) {

		for (byte i = 0; i < numSprites; i++) {
			// Height is 8
			if (!PPUCTRL.H) {

				if (!(sprites[i].attribute & 0x80)) {

					spritePatternALow = (PPUCTRL.S << 12) | (sprites[i].id << 4) | (scanline - sprites[i].y);
				}

				else {

					spritePatternALow = (PPUCTRL.S << 12) | (sprites[i].id << 4) | (7 - scanline - sprites[i].y);
				}
			}

			else {

				if (!(sprites[i].attribute & 0x80)) {

					// Top 8 pixel rows of sprite
					if (scanline - sprites[i].y < 8) {

						spritePatternALow = ((sprites[i].id & 0x01) << 12) | ((sprites[i].id & 0xFE) << 4) | ((scanline - sprites[i].y) & 0x07);
					}

					else {

						spritePatternALow = ((sprites[i].id & 0x01) << 12) | ((sprites[i].id & 0xFE + 1) << 4) | ((scanline - sprites[i].y) & 0x07);
					}
				}

				else {

					if (scanline - sprites[i].y < 8) {

						spritePatternALow = ((sprites[i].id & 0x01) << 12) | ((sprites[i].id & 0xFE + 1) << 4) | (7 - (scanline - sprites[i].y) & 0x07);
					}

					else {

						spritePatternALow = ((sprites[i].id & 0x01) << 12) | ((sprites[i].id & 0xFE) << 4) | (7 - (scanline - sprites[i].y) & 0x07);
					}
				}
			}

			// Read from PPU bus to get bits
			spritePatternAHigh = spritePatternALow + 8;

			spritePatternBLow = readPPUBus(spritePatternALow);
			spritePatternBHigh = readPPUBus(spritePatternAHigh);

			// Flip if needed
			if (sprites[i].attribute & 0x40) {

				spritePatternBLow = flipBits(spritePatternBLow);
				spritePatternBHigh = flipBits(spritePatternBHigh);

			}
			lowSprites[i] = spritePatternBLow;
			highSprites[i] = spritePatternBHigh;
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
		}
	}


	if (PPUMASK.s) {

		if (PPUMASK.M || (PPUcycle > 8)) {

			spriteZeroRender = false;

			for (byte i = 0; i < numSprites; i++) {

				if (sprites[i].x == 0) {
					byte tempLowPix = (lowSprites[i] & 0x80) > 0;
					byte tempHighPix = (highSprites[i] & 0x80) > 0;
					spritePix = (tempHighPix << 1) | tempLowPix;
					spritePalette = (sprites[i].attribute & 0x03) + 0x04;
					spritePrio = (sprites[i].attribute & 0x20) == 0;

					if (spritePix != 0) {

						if (i == 0) {

							spriteZeroRender = true;
						}

						break;
					}
				}
			}
		}
	}

	if (pixel == 0 && spritePix == 0) {

		totalPixel = 0;
		totalPalette = 0;
	}

	else if (pixel == 0 && spritePix > 0) {

		totalPixel = spritePix;
		totalPalette = spritePalette;
	}

	else if (pixel > 0 && spritePix == 0) {

		totalPixel = pixel;
		totalPalette = palette;
	}

	else if (pixel > 0 && spritePix > 0) {

		if (spritePrio) {

			totalPixel = spritePix;
			totalPalette = spritePalette;
		}

		else {

			totalPixel = pixel;
			totalPalette = palette;
		}

		if (spriteZeroRender && canSpriteZero) {

			if (PPUMASK.b && PPUMASK.s) {

				if (!(PPUMASK.M || PPUMASK.m)) { // Either

					if (PPUcycle > 8 && PPUcycle < 258) {

						PPUSTATUS.S = 1;
					}
				}

				else {

					if (PPUcycle > 0 && PPUcycle < 258) {

						PPUSTATUS.S = 1;
					}
				}
			}
		}
	}

	uint16_t finalAddr = readPPUBus(0x3F00 + totalPixel + 4 * totalPalette); //(0x3F00 | ((palette << 2) | pixel));

	if (pixel != 0 && temp) {
			temp = false;			
	}



	SetPixel(PPUcycle - 1, scanline, pixelColors[finalAddr]);

	if (PPUcycle == 340 && scanline != 260 ) {

		PPUcycle = -1; // This is -1 because PPUcycle is incremented directly after this
		scanline++;
	}

	if (scanline == 260 && PPUcycle == 340) {
		UpdateScreen();
		PPUcycle = 0;
		scanline = -1;
		printDebug = false;
		frameDone = true;
	}

	PPUcycle++;
}

