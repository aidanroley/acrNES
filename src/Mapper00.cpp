#include "Mapper00.h"
#include <iostream>

// Mapper00::Mapper00(Bus* bus) : bus(bus) {}

void Mapper00::mapperCpuRead() {
    
}

void Mapper00::mapperCpuWrite(const std::vector<uint8_t>& prg, const std::vector<uint8_t>& chr) {
//busmemory[0] = 1;
   
}

void Mapper00::mapperPpuRead() {
    
}

void Mapper00::mapperPpuWrite() {
    
}

void Mapper00::initializePRG(const std::vector<uint8_t>& prg) {
    for (size_t i = 0; i < prg.size(); ++i) {
        //bus->
    }

}