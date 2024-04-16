#ifndef BUS_H // If BUS_H is not defined
#define BUS_H // Define BUS_H

#include <cstdint> // Include for uint8_t definition
#include <vector> // If you use std::vector
#include <string> // If you use std::string
#include <functional> // If you use std::function
#include <unordered_map> // If you use std::unordered_map
#include <iostream>
#include <iomanip> 
#include "singleton.h"

class PPU;
class Mapper00;

typedef uint8_t byte;

class Bus : public Singleton<Bus> {
    friend class Singleton<Bus>;
private:
    Bus();
    PPU* ppu; 
    Mapper00* mapper00;
public:
    uint8_t memory[0x10000]; // Fixed size to 0x4000 which is 16KB, not 64KB
    // Bus();
    void connectPPU(PPU* ppu);
    void connectMapper00(Mapper00* mapper00);
    void dmaTransfer(uint16_t startAddr, byte* OAM, size_t size);


    void initializePRG(const std::vector<uint8_t>& prg);
    void check();

   

    // Read-Only
    uint8_t readBusCPU(uint16_t address);

    // CPU address
    void writeBusCPU(uint16_t address, uint8_t data);
};

#endif // BUS_H
