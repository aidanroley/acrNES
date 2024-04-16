#include "Mapper00.h"
#include <iostream>

// Mapper00::Mapper00(Bus* bus) : bus(bus) {}



void Bus::initializePRGM0(const std::vector<uint8_t>& prg) {
    size_t prgSize = prg.size();



    // Ensure the PRG ROM fits within the allocated memory space
    if (0x8000 + prgSize > sizeof(memory)) {
        std::cerr << "PRG ROM size exceeds available memory space." << std::endl;
        return;
    }

    for (size_t i = 0; i < prgSize; ++i) {
        memory[0x8000 + i] = prg[i];
    }

    // Handle mirroring if PRG is exactly 16KB
    if (prgSize == 16384) {
        for (size_t i = 0; i < prgSize; ++i) {
            memory[0x8000 + 0x4000 + i] = prg[i];
        }
    }

    /* Print the loaded PRG ROM data from memory starting at 0x8000
    std::cout << "Loaded PRG ROM Data in Memory (starting at 0x8000):" << std::endl;
    for (size_t i = 0; i < prgSize; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(memory[0x8000 + i]) << ' ';
        if ((i + 1) % 16 == 0)  // Newline every 16 bytes for better readability
            std::cout << std::endl;
    }
    std::cout << std::endl;
    */

}

void PPU::initializeCHRM0(const std::vector<uint8_t>& chr) {
    ppuCHR = chr;
}
void Mapper00::mapperCpuRead() {
    
}

void Mapper00::mapperCpuWrite(const std::vector<uint8_t>& prg, const std::vector<uint8_t>& chr) {
//busmemory[0] = 1;
   
}

void Mapper00::mapperPpuRead() {
    
}

void Mapper00::mapperPpuWrite() {
    
}

