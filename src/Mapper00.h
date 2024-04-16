#pragma once
#include <vector>
#include <cstdint>
#include <iostream>
#include "bus.h"
#include "cpu.h"
#include "PPU.h"

//class Bus;

class Mapper00 {
public:
    Bus* bus = Bus::getInstance();
    ppuBus* ppuBus = ppuBus::getInstance();

    Mapper00(const std::vector<uint8_t>& prg, const std::vector<uint8_t>& chr)
        : PRGarray(prg), CHRarray(chr) {
        std::cout << "PRG SIZE IN MAPPER " << PRGarray.size()
            << " CHR SIZE IN MAPPER " << CHRarray.size() << std::endl;
        bus->initializePRG(prg);
        ppuBus->initializeCHR(chr);
    }
    
    byte prgStart = 0x8000;
    // void setBus(Bus* b) { bus = b; }
    void initializePRG(const std::vector<uint8_t>& prg);
    void mapperCpuRead();
    void mapperCpuWrite(const std::vector<uint8_t>& prg, const std::vector<uint8_t>& chr);
    void mapperPpuRead();
    void mapperPpuWrite();

private:
    std::vector<uint8_t> PRGarray;
    std::vector<uint8_t> CHRarray;
};
