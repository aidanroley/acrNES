#pragma once
#include <vector>
#include <cstdint>
#include <iostream>
#include "bus.h"
#include "cpu.h"
#include "PPU.h"

class Mapper00 {
public:

    // Get the synchronized instance of Bus/PPU
    Bus* bus = Bus::getInstance();
    PPU* PPU = PPU::getInstance();
     
    // Put PGR in the bus, put CHR in PPU, pass mirror orientation into PPU 
    Mapper00(const std::vector<uint8_t>& prg, const std::vector<uint8_t>& chr, bool mirror) // if mirror, horizontal
        : PRGarray(prg), CHRarray(chr) {
        std::cout << "PRG SIZE IN MAPPER " << PRGarray.size()
            << " CHR SIZE IN MAPPER " << CHRarray.size() << std::endl;
        bus->initializePRGM0(prg);
        PPU->initializeCHRM0(chr);
        PPU->mirror(mirror);
    }
    
    byte prgStart = 0x8000;
    void mapperCpuRead();
    void mapperCpuWrite(const std::vector<uint8_t>& prg, const std::vector<uint8_t>& chr);
    void mapperPpuRead();
    void mapperPpuWrite();

private:
    std::vector<uint8_t> PRGarray;
    std::vector<uint8_t> CHRarray;
};
