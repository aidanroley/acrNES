#include "bus.h"
#include "PPU.h"

Bus::Bus() {
    // Initialize 64KB memory to zero
    for (int i = 0; i < 65536; i++) {
        memory[i] = 0;
    }
}

void Bus::connectPPU(PPU* ppu) {
    this->ppu = ppu;
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
