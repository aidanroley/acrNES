#pragma once
#include <vector>
#include <cstdint>
#include <iostream>
#include "bus.h"
#include "ppuBus.h"
#include "cpu.h"
#include "ppu.h"

// Forward declaration
// class Bus;

class Mapper00 {
public:

    Mapper00(const std::vector<uint8_t>& prg, const std::vector<uint8_t>& chr)
        : PRGarray(prg), CHRarray(chr) {
        std::cout << "PRG SIZE IN MAPPER " << PRGarray.size()
            << " CHR SIZE IN MAPPER " << CHRarray.size() << std::endl;
    }
    

    // void setBus(Bus* b) { bus = b; }

    void mapperCpuRead();
    void mapperCpuWrite();
    void mapperPpuRead();
    void mapperPpuWrite();

private:
    // Bus* bus;
    std::vector<uint8_t> PRGarray;
    std::vector<uint8_t> CHRarray;
};
