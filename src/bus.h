#ifndef BUS_H // If BUS_H is not defined
#define BUS_H // Define BUS_H

#include <cstdint>
#include <vector> // For Lookup Table
#include <string>
#include <functional>
#include <unordered_map>
class Bus {
public:
    uint8_t memory[65536];

    Bus() {
        // 64KB memory, all set to zero initially
        for (int i = 0; i < 65536; ++i) {
            memory[i] = 0;
        }
    }

    //Read-Only
    uint8_t readBus(uint16_t address) const {
        return memory[address];
    }

    void writeBus(uint16_t address, uint8_t data) {
        memory[address] = data;
    }




};
#endif