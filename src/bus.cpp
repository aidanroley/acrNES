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
       // return ppu->handlePPURead(ppuRegister);
    }
    return 0xFF; // Default if not in handled ranges
}

void Bus::writeBusCPU(uint16_t address, uint8_t data) {
    if (address >= 0x0000 && address <= 0x1FFF) {
        memory[address & 0x07FF] = data; // Mirror every 2KB
    }
    else if (address >= 0x2000 && address <= 0x3FFF) {
        uint16_t ppuRegister = address & 0x0007; // Mask to get PPU register
       // ppu->handlePPUWrite(ppuRegister, data);
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


void Bus::check() {
    std::cout << "INSTANCE CHECKER" << std::endl;
    for (size_t i = 0; i < 16384; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(memory[0x8000 + i]) << ' ';
        if ((i + 1) % 16 == 0)  // Newline every 16 bytes for better readability
            std::cout << std::endl;
    }
}

uint16_t Bus::CpuPcStart(uint16_t pc) {
    // Handle starting position of CPU's PC
    lowPCStart = memory[0xFFFC];
    highPCStart = memory[0xFFFD];
    PCStart = (highPCStart << 8) | lowPCStart;
    return PCStart;
}

void Bus::storeTempValues(uint16_t operandAddress, byte operandValue, int cycleCount) {
    cpuTempAddr = operandAddress;
    cpuTempData = operandValue;
    cpuTempCycles = cycleCount;
}

void Bus::busClock() {
    std::cout << "yeah" << std::endl;
    if (ppuCycles == 0) {
        // cpu->run();
        std::cout << "yea" << std::endl;
        cpuCycles++;
    }
    std::cout << "awesome sauce" << std::endl;
    ppu->clock();
    ppuCycles++;
   

    // PPU runs 3 times as fast as CPU
    if (cpuTempCycles == 0 && (ppuCycles % 3 == 0)) {

        cpu->run();
        cpuCycles++;
    }

    // Only write to it when its one cycle from being done
    else if (cpuTempCycles == 1) {

        writeBusCPU(cpuTempAddr, cpuTempData);
    }

    else if (cpuTempCycles > 1) {   
        
        // If cycles > 0, we need to wait before fetching next instruction
    }

    ppu->clock();
    ppuCycles++;


    // idk if I should decrement right after cpu run
    cpuTempCycles--;

}
