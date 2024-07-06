#ifndef BUS_H // If BUS_H is not defined
#define BUS_H // Define BUS_H

// #include "PPU.h"
#include <cstdint> // Include for uint8_t definition
#include <vector> // If you use std::vector
#include <string> // If you use std::string
#include <functional> // If you use std::function
#include <unordered_map> // If you use std::unordered_map
#include <iostream>
#include <iomanip> 
#include "singleton.h"
#include <SDL.h>;

class PPU;
class Mapper00;
class cpu;
class Emulator;

typedef uint8_t byte;

class Bus : public Singleton<Bus> {
    friend class Singleton<Bus>;
private:
    Bus();

public:
    PPU* ppu;
    Mapper00* mapper00;
    cpu* cpu;
    Emulator* emulator;

    uint8_t memory[0x10000]; // Fixed size to 0x4000 which is 16KB, not 64KB
    // Bus();
    void connectPPU(PPU* ppu);
    void connectMapper00(Mapper00* mapper00);
    void dmaTransfer(byte startAddr);
   byte dmaStartAddr;
    byte dmaData;
    byte dmaAddr;


    void initializePRGM0(const std::vector<uint8_t>& prg);
    void check();

    // For finding CPU's PC start location
    uint16_t CpuPcStart();
    byte lowPCStart;
    byte highPCStart;
    uint16_t PCStart;

   

    // Read-Only
    uint8_t readBusCPU(uint16_t address);

    // CPU address
    void writeBusCPU(uint16_t address, uint8_t data);

    void busClock();

    // For cycles
    int cpuTempCycles = 0;
    byte cpuTempData;
    uint16_t cpuTempAddr;
    int ppuCycles = 0;
    int cpuCycles = 0;
    bool dontDecrement = false;

    void storeTempValues(uint16_t operandAddress, byte operandValue, int cycles);

    void transferCycles(int cycleCount);

    bool write = false;

    bool nmi = false;

    bool dma = false;
    bool dmaTemp = false;
    int dmaCycles;

    void transferNMI();

    byte tempInputRegister;
    byte inputRegister;
    byte actualInputRegister;
    
    void updateControllerState(const SDL_Event& e) {
        //inputRegister = 0;
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {

            bool pressed = (e.type == SDL_KEYDOWN);

            switch (e.key.keysym.sym) {

            case SDLK_RIGHT://x:
                if (pressed) {
                    inputRegister |= 0x01;
                }
                else {
                    inputRegister &= ~0x01;
                };
                break;

            case SDLK_LEFT://z:
                if (pressed) {
                    inputRegister |= 0x02;
                }
                else {
                    inputRegister &= ~0x02;
                }
                break;

            case SDLK_DOWN://a:
                if (pressed) {
                    inputRegister |= 0x04;
                }
                else {
                    inputRegister &= ~0x04;
                }
                break;

            case SDLK_UP://s:
                if (pressed) {
                    inputRegister |= 0x08;
                }
                else {
                    inputRegister &= ~0x08;
                }
                break;

            case SDLK_s://UP:
                if (pressed) {
                    inputRegister |= 0x10;
                }
                else {
                    inputRegister &= ~0x10;
                }
                break;

            case SDLK_a://DOWN:
                if (pressed) {
                    inputRegister |= 0x20;
                }
                else {
                    inputRegister &= ~0x20;
                }
                break;

            case SDLK_z://LEFT:
                if (pressed) {
                    inputRegister |= 0x40;
                }
                else {
                    inputRegister &= ~0x40;
                }
                break;

            case SDLK_x: //RIGHT:
                if (pressed) {
                    inputRegister |= 0x80;
                }
                else {
                    inputRegister &= ~0x80;
                }
                break;
            }

        }
    }
    byte previousValue = 0;
};

#endif // BUS_H
