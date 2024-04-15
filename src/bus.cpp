#include "bus.h"

Bus::Bus() {
    // Initialize 64KB memory to zero
    for (int i = 0; i < 65536; i++) {
        memory[i] = 0;
    }
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
}
