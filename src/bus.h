#ifndef BUS_H // If BUS_H is not defined
#define BUS_H // Define BUS_H

#include "ROM.h"
#include "ppu.h" // Assuming this is needed for the ppu object
#include <cstdint> // Include for uint8_t definition
#include <vector> // If you use std::vector
#include <string> // If you use std::string
#include <functional> // If you use std::function
#include <unordered_map> // If you use std::unordered_map

class Bus {
private:
    ppu ppu; // Ensure ppu is defined correctly in "ppu.h"
public:
    uint8_t memory[0x10000]; // Fixed size to 0x4000 which is 16KB, not 64KB

    Bus();

    // Read-Only
    uint8_t readBusCPU(uint16_t address);

    // CPU address
    void writeBusCPU(uint16_t address, uint8_t data);
};

#endif // BUS_H
