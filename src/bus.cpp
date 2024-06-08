#include "bus.h"
#include "PPU.h"
#include "cpu.h"
#include "Mapper00.h"
#include "Emulator.h"


Bus::Bus() : ppu(nullptr), mapper00(nullptr), cpu(nullptr), emulator(nullptr) {
    std::fill(std::begin(memory), std::end(memory), 0);

    //Bus::connectPPU(ppu);
    // Optionally log that the bus has been initialized
    std::cout << "Bus initialized with zeroed memory and no connected PPU or Mapper." << std::endl;
}


void Bus::connectPPU(PPU* ppu) {
    ppu = PPU::getInstance();
}
void Bus::connectMapper00(Mapper00* mapper00) {
    this->mapper00 = mapper00;  
}

uint8_t Bus::readBusCPU(uint16_t address) {
    if (address >= 0x0000 && address <= 0x1FFF) {
        return memory[address & 0x07FF]; // Mirror every 2KB
    }
    else if (address >= 0x2000 && address <= 0x3FFF) {
        uint16_t ppuRegister = address & 0x0007 + 0x2000; // Mask to get PPU register
        return ppu->handlePPURead(ppuRegister);
    }
    else if (address == 0x4016) {
        byte inputData = (inputRegister & 0x80) != 0 ? 0x01 : 0x00;
        inputRegister <<= 1;
        return inputData;
    }
    else if (address >= 0x4017 && address <= 0xFFFF) {
        //std::cout << "bad. " << address << std::endl;
        return memory[address];
    }
    return 0x00; // Default if not in handled ranges
}

void Bus::writeBusCPU(uint16_t address, uint8_t data) {
    if (address >= 0x0000 && address <= 0x1FFF) {
        memory[address & 0x07FF] = data; // Mirror every 2KB
    }
    else if (address >= 0x2000 && address <= 0x3FFF) {
        // std::cout << "INCREDIBLE!!!!!" << std::endl;
        uint16_t ppuRegister = address & 0x0007 + 0x2000; // Mask to get PPU register

        ppu->handlePPUWrite(ppuRegister, data);
    }
    else if (address == 0x4014) {
        ppu->handlePPUWrite(address, data);
        // MAKE CPU WAIT 512 CYCLES :D
    }
    else if (address == 0x4016) {

        if (data & 0x01) {
            previousValue = 1;
        }

        else if (previousValue == 1) { //&& data == 0) {
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    emulator->ok = false;
                }
                else {
                    updateControllerState(e);
                    previousValue = 0;

                }
            }
        }
        
        //previousValue = data;
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

void Bus::transferNMI() {
    nmi = true;
}


void Bus::check() {
    /*
    std::cout << "INSTANCE CHECKER" << std::endl;
    for (size_t i = 0; i < 16384; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(memory[0x8000 + i]) << ' ';
        if ((i + 1) % 16 == 0)  // Newline every 16 bytes for better readability
            std::cout << std::endl;
    }
    */
}

uint16_t Bus::CpuPcStart() {
    // Handle starting position of CPU's PC
    cpu->initAddressingModeHandlers();
    lowPCStart = memory[0xFFFC];
    highPCStart = memory[0xFFFD];
    PCStart = (highPCStart << 8) | lowPCStart;
    std::cout << "Low byte: " << std::hex << static_cast<int>(lowPCStart) << std::endl;
    std::cout << "High byte: " << std::hex << static_cast<int>(highPCStart) << std::endl;
    std::cout << "PCStart: " << std::hex << PCStart << std::endl;
    return PCStart;
    //return 0xC000;
}
void Bus::transferCycles(int cycleCount) {
    cpuTempCycles = cycleCount;

}
void Bus::storeTempValues(uint16_t operandAddress, byte operandValue, int cycleCount) {
    
    cpuTempAddr = operandAddress;
    cpuTempData = operandValue;
    cpuTempCycles = cycleCount;
    //cpuCycles += cpuTempCycles;
    write = true;
}

void Bus::busClock() {
    if (ppuCycles == 999) {
        ppuCycles = 3;
    }
    if (ppuCycles == 0) {
        ppu = PPU::getInstance();
        cpu = cpu::getInstance();
    }
    
    if (ppuCycles == 0) {
        cpu->setPCStartup();
        cpu->run();
        //cpuCycles++;
    }
    /*
    ppu->clock();
    std::cout << "PPU clocked" << std::endl;
    ppuCycles++;
    */
   
   //std::cout << " CPU TEMP CYCLES: " << cpuTempCycles << std::endl;
    // std::cout << " PPU TEMP CYCLES: " << ppuCycles << std::endl;
    // PPU runs 3 times as fast as CPU
    if (cpuTempCycles == 0 && (ppuCycles % 3 == 0)) {

        cpu->run();
       // std::cout << "CPU clocked" << std::endl;
        // cpuCycles++;
    }

    // Only write to it when its one cycle from being done
    else if (cpuTempCycles == 1 && write) {

        writeBusCPU(cpuTempAddr, cpuTempData);
        write = false;
    }

    ppu->clock();
    // std::cout << "PPU clocked" << std::endl;
    ppuCycles++;

    if (nmi) {
        cpu->nmi();
        nmi = false;
    }
    /*
    if (ppuCycles == 400000) {
        for (const auto& value : ppu->ppuVRAM) {
            //std::cout << "02A0 should be 2A" << static_cast<int>(ppu->ppuVRAM[0x02A0]) << std::endl;
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(value) << " ";
            
        }
        exit(0);
    }
    */


    // idk if I should decrement right after cpu run
    if (cpuTempCycles > 0 && (ppuCycles % 3 == 0)) {
        cpuTempCycles--;
    }

}
