#pragma once

#ifndef ROM_H
#define ROM_H
#include "Mapper00.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <vector>



class ROM {
private:

public:
    // Header declarations
    int parseFile();
    

    struct NESHeader {
        bool iNES;
        bool NES2;

        int prgPages;
        int chrPages;
        byte ROMFlags = 0;

        // Booleans for byte 6 of header
        bool mirror;
        bool battery;
        bool trainer;
        bool fourscreen;
        byte mapperLowerNybble;

        // Booleans for byte 7 of header
        bool VS;
        bool PlayChoice;
        byte mapperUpperNybble;

        int mapperNumber;

        // For byte 8
        byte prgRamSize;

        // For byte 9
        bool tvSystem;

        // For byte 10
        byte tvSystemByte;
        bool prgRam;
        bool busConflicts;

        // Array and vectors that hold data from the cartridge
        byte trainerArray[512];
        std::vector<byte> PRGarray;
        std::vector<byte> CHRarray;

        int byteCounter;
    };
    NESHeader header;

    enum MirroringType {
        HORIZONTAL,
        VERTICAL,
        FOUR_SCREEN
    };

    enum TVSystem {
        NTSC,
        PAL,
        DUAL
    };

    void loadMapper(int mapperNumber);

    
};

#endif // ROM_H
