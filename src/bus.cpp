#include "bus.h"
#include "PPU.h"
#include "cpu.h"
#include "Mapper00.h"
#include "Emulator.h"


Bus::Bus() : ppu(nullptr), mapper00(nullptr), cpu(nullptr), emulator(nullptr) {
    std::fill(std::begin(memory), std::end(memory), 0);
    
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
        byte inputData = (actualInputRegister & 0x80) > 0;
        actualInputRegister <<= 1;
        return inputData;
    }
    else if (address >= 0x4017 && address <= 0xFFFF) {
        return memory[address];
    }
    return 0x00; // Default if not in handled ranges
}

void Bus::writeBusCPU(uint16_t address, uint8_t data) {
    if (address >= 0x0000 && address <= 0x1FFF) {
        memory[address & 0x07FF] = data; // Mirror every 2KB
    }
    else if (address >= 0x2000 && address <= 0x3FFF) {
        uint16_t ppuRegister = address & 0x0007 + 0x2000; // Mask to get PPU register

        ppu->handlePPUWrite(ppuRegister, data);
    }
    else if (address == 0x4014) {
        dmaTransfer(data);
    }
    else if (address == 0x4016) {
        actualInputRegister = inputRegister;
    }

}


void Bus::dmaTransfer(byte startAddr) {
    dma = true;
    dmaTemp = true;
    dmaCycles = 512;
    dmaStartAddr = startAddr;
    dmaAddr = 0;

}

void Bus::transferNMI() {
    nmi = true;
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
    write = true;
}

void Bus::busClock() {
    if (ppuCycles == 999999) {
        ppuCycles = 40000;
    }
    if (ppuCycles == 0) {
        ppu = PPU::getInstance();
        cpu = cpu::getInstance();
    }
    
    if (ppuCycles == 0) {
        cpu->setPCStartup();
        cpu->run();
    }

    // PPU runs 3 times as fast as CPU
    if (dma && ppuCycles % 3 == 0) {
        

        if (dmaTemp) {

            if (ppuCycles % 2 == 1) {

                dmaTemp = false;
            }
        }
        else {

            if (ppuCycles % 2 == 0) {

                dmaData = readBusCPU(dmaStartAddr << 8 | dmaAddr);
            }

            else {
                ppu->OAM[dmaAddr] = dmaData; 
                dmaAddr++;

                if (dmaAddr >= 256) {

                    dmaAddr = 0;
                }

                if (dmaAddr == 0) {

                    dma = false;
                }
            }
        }


    }
    if (!dma && cpuTempCycles == 0 && (ppuCycles % 3 == 0)) {

        cpu->run();

    }

    // Only write to it when its one cycle from being done
    else if (!dma && cpuTempCycles == 1 && write) {

        writeBusCPU(cpuTempAddr, cpuTempData);
        write = false;
    }

    ppu->clock();
    ppuCycles++;

    if (nmi && !dma) {
        cpu->nmi();
        nmi = false;
    }


    if (cpuTempCycles > 0 && (ppuCycles % 3 == 0) && !dma) {
        cpuTempCycles--;
    }

}
