#include "Mapper00.h"
#include <iostream>

void Bus::initializePRGM0(const std::vector<uint8_t>& prg) {
    size_t prgSize = prg.size();

    // Ensure the PRG ROM fits within the allocated memory space
    if (0x8000 + prgSize > sizeof(memory)) {
        std::cerr << "PRG ROM size exceeds available memory space." << std::endl;
        return;
    }

    // 0x8000 is the starting location of PRG data
    for (size_t i = 0; i < prgSize; ++i) {
        memory[0x8000 + i] = prg[i];
    }

    // Handle mirroring if PRG is exactly 16KB
    if (prgSize == 16384) {
        for (size_t i = 0; i < prgSize; ++i) {
            memory[0x8000 + 0x4000 + i] = prg[i];
        }
    }

}

void PPU::initializeCHRM0(const std::vector<uint8_t>& chr) {
    for (int i = 0; i < chr.size(); ++i) {
        ppuVRAM[i] = chr[i];
    }
}
void Mapper00::mapperCpuRead() {
    
}

void Mapper00::mapperCpuWrite(const std::vector<uint8_t>& prg, const std::vector<uint8_t>& chr) {
   
}

void Mapper00::mapperPpuRead() {
    
}

void Mapper00::mapperPpuWrite() {
    
}

