#include "bus.h"
#include "PPU.h"
#include "Mapper00.h"

Bus::Bus() : ppu(nullptr), mapper00(nullptr) {
    std::fill(std::begin(memory), std::end(memory), 0);

    // Optionally log that the bus has been initialized
    std::cout << "Bus initialized with zeroed memory and no connected PPU or Mapper." << std::endl;
}


void Bus::connectPPU(PPU* ppu) {
    this->ppu = ppu;
}
void Bus::connectMapper00(Mapper00* mapper00) {
    this->mapper00 = mapper00;  
}

uint8_t Bus::readBusCPU(uint16_t address) {
    if (address >= 0x0000 && address <= 0x1FFF) {
        return memory[address & 0x07FF]; // Mirror every 2KB
    }
    else if (address >= 0x2000 && address <= 0x3FFF) {
        uint16_t ppuRegister = address & 0x0007; // Mask to get PPU register
       // return ppu.handlePPURead(ppuRegister);
    }
    return 0xFF; // Default if not in handled ranges
}

void Bus::writeBusCPU(uint16_t address, uint8_t data) {
    if (address >= 0x0000 && address <= 0x1FFF) {
        memory[address & 0x07FF] = data; // Mirror every 2KB
    }
    else if (address >= 0x2000 && address <= 0x3FFF) {
        uint16_t ppuRegister = address & 0x0007; // Mask to get PPU register
       // ppu.handlePPUWrite(ppuRegister, data);
    }
    else if (address == 0x4014) {
        ppu->handlePPUWrite(address, data);
        // MAKE CPU WAIT 512 CYCLES :D
    }
}

void Bus::dmaTransfer(uint16_t startAddr, byte* OAM, size_t size) {
    std::cout << "OAM Data: \n";
    for (size_t i = 0; i < size; ++i) {
        OAM[i] = memory[startAddr + i];
        /*
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(OAM[i]) << ' ';
        if ((i + 1) % 16 == 0) // Newline every 16 bytes for better readability
            std::cout << std::endl;
            */
    }


}
void Bus::initializePRG(const std::vector<uint8_t>& prg) {
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

void Bus::check() {
    std::cout << "INSTANCE CHECKER" << std::endl;
    for (size_t i = 0; i < 16384; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(memory[0x8000 + i]) << ' ';
        if ((i + 1) % 16 == 0)  // Newline every 16 bytes for better readability
            std::cout << std::endl;
    }
}

