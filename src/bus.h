#ifndef BUS_H // If BUS_H is not defined
#define BUS_H // Define BUS_H
#include "ppu.h"
#include <vector> // For Lookup Table
#include <string>
#include <functional>
#include <unordered_map>
class Bus {
private:
    ppu ppu;
public:
    uint8_t memory[0x3FFF];

    Bus() {
        // 64KB memory, all set to zero initially
        for (auto& i : memory) {
            memory[i] = 0;
        }
    }

    //Read-Only
    uint8_t readBusCPU(uint16_t address) {
        uint16_t ppuRegister;
        uint8_t data = 0xFF;
        if (address >= 0x0000 && address <= 0x1FFF) {
            data = memory[address & 0x07FF];
        }
        else if (address >= 0x2000 && address <= 0x3FFF) {
            ppuRegister = 0x2000 + (address & 0x0007);
            ppu.handlePPURead(ppuRegister);
        }
        return data;
    }

    // CPU address
    void writeBusCPU(uint16_t address, uint8_t data) {
        uint16_t ppuRegister;
        if (address >= 0x0000 && address <= 0x1FFF) {
            memory[address & 0x07FF] = data;
        }
        else if (address >= 0x2000 && address <= 0x3FFF) {
            ppuRegister = 0x2000 + (address & 0x0007);
            ppu.handlePPUWrite(ppuRegister);
        }
        
    }


};
#endif