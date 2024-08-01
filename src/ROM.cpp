#include "ROM.h"

byte output;  
// ROM rom;


int ROM::parseFile() {
	// NESHeader header;
	header.iNES = false;
	header.NES2 = false;
	std::vector<byte> byteArray;
	const char* filename = "C:\\Users\\bridg\\Downloads\\Super Mario Bros (E)\\Super Mario Bros (E).nes";
	//const char* filename = "C:\\Users\\bridg\\Downloads\\donkeykong.nes";
	//const char* filename = "C:\\Users\\bridg\\Downloads\\nestest.nes";
	//const char* filename = "C:\\Users\\bridg\\Downloads\\cpu_dummy_reads.nes";
	// const char* filename = "C:\\Users\\bridg\\Downloads\\Battletoads (USA).nes";
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		std::cerr << "Cannot open file." << std::endl;
	}

	while (file.read(reinterpret_cast<char*>(&output), sizeof(output))) {
		byteArray.push_back(output);
	}

	if (byteArray[0] == 0x4E && byteArray[1] == 0x45 && byteArray[2] == 0x53 && byteArray[3] == 0x1A) {
		header.iNES = true;

	}
	if (header.iNES == true && (byteArray[7] & 0x0c) == 0x08) {
		header.NES2 = true;

	}
	// 4th Byte (5th if you include 0th)
	header.prgPages = (static_cast<unsigned int>(byteArray[4]));
	header.PRGarray.reserve(header.prgPages * 16384);
	std::cout << "Number of 16KB PRG ROM pages: " << static_cast<unsigned int>(byteArray[4]) << std::endl;



	// 5th Byte
	header.chrPages = (static_cast<unsigned int>(byteArray[5]));
	header.CHRarray.reserve(header.chrPages * 8192);
	std::cout << "Number of 8KB CHR ROM pages: " << static_cast<unsigned int>(byteArray[5]) << std::endl;

	// 6th Byte
	byte flags6 = byteArray[6];
	header.mirror = flags6 & 0x01; // mirror ? horizontal : vertical
	header.battery = flags6 & 0x02; // battery ? yes battery: none
	header.trainer = flags6 & 0x04; // trainer ? yes trainer : none
	header.fourscreen = flags6 & 0x08; // fourscreen ? yes fourscreen : none
	header.mapperLowerNybble = flags6 >> 4; // lower 4 bits of mapper number

	// 7th Byte
	byte flags7 = byteArray[7];
	header.VS = flags7 & 0x01;
	header.PlayChoice = flags7 & 0x02;
	header.NES2 = ((flags7 >> 2) & 0x03) == 0x02;
	header.mapperUpperNybble = flags7 >> 4; // upper 4 bits of mapper number

	header.mapperNumber = (header.mapperUpperNybble << 4) | (header.mapperLowerNybble);

	// 8th Byte
	header.prgRamSize = (static_cast<unsigned int>(byteArray[8])) * 8192;
	

	// 9th Byte
	header.tvSystem = byteArray[9] & 0x01; // tvSystem ? PAL : NTSC

	// 10th Byte
	byte flags10 = byteArray[10];
	header.tvSystemByte = (flags10 & 0x02) | (flags10 & 0x01);
	header.prgRam = !(flags10 & 0x10); // prgRam ? not present : present (why is 1 !present Idk source: https://www.nesdev.org/wiki/INES) hence the negation to make it easier to understand
	header.busConflicts = flags10 & 0x20; // busConflicts ? present : not present

	header.byteCounter = 16; // Skip to byte 16 because of padding

	// If trainer is present, fill the trainer array with the next 512 bytes starting at byte 11
	if (header.trainer) {
		for (int i = 16; i < 528; i++) {
			header.trainerArray[i - 16] = byteArray[i];
		}
		header.byteCounter = 528; // If there is a trainer this needs to be past it (528)
	}

	// Fill the Program Vector with PRG data
	for (int i = 0; i < header.prgPages * 16384; i++) {
		header.PRGarray.push_back(byteArray[i + header.byteCounter]);
}
	header.byteCounter += header.PRGarray.size();

	if (header.chrPages != 0) {
		for (int i = 0; i < header.chrPages * 8192; i++) {
			header.CHRarray.push_back(byteArray[i + header.byteCounter]);
		}
		header.byteCounter += header.CHRarray.size();
	}


	std::cout << "Initial Byte Counter: " << header.byteCounter << std::endl;

	std::cout << "PRG Size: " << header.PRGarray.size() << std::endl;
	std::cout << "CHR Size: " << header.CHRarray.size() << std::endl;

	std::cout << "Mapper Number: " << header.mapperNumber << std::endl;
		loadMapper(header.mapperNumber);
		
	file.close();
	return 0;
}

void ROM::loadMapper(int mapperNumber) {
	switch(mapperNumber) {
	case 0: { Mapper00 mapper(header.PRGarray, header.CHRarray, header.mirror); break; }
	case 1: break;
	}

}