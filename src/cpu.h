#pragma once
#ifndef CPU_H
#define CPU_H 
#include "bus.h"

// typedef uint8_t byte;

class cpu : public Singleton<cpu> {
    friend class Singleton<cpu>;
private:
    Bus* bus = Bus::getInstance();



public:
    cpu();
    ~cpu();

    // 8-bit accumulators, status register, stack pointer
    byte a = 0, x = 0, y = 0, status = 0, stackpt = 0xFD;

    // 16-bit program counter; it points to the address at which next instruction will be fetched
    // The program counter may be read by pushing its value onto the stack  
    uint16_t pc;

    uint16_t absolute;
    uint16_t temp_addr;
    byte operand;
    byte zeroPageAddr;
    int8_t relative;
    uint16_t jump; uint16_t tempjump;


    using AddressingModeHandler = std::function <uint16_t()>;

    // Flag bits
    enum flags {
        N = 1 << 7, // Negative
        V = 1 << 6, // Overflow
        U = 1 << 5, // Unused
        B = 1 << 4, // Break
        D = 1 << 3, // Decimal (Not used in NES)
        I = 1 << 2, // Interrupt Disable
        Z = 1 << 1, // Zero
        C = 1 << 0, // Carry
    };

    // Addressing modes
    // Added e so it doesn't conflict w/ lookup table (e for execute)
    byte IMPe();
    byte ACCe();
    byte IMMe();
    byte ZEROe();
    byte ZEROXe();
    byte ZEROYe();
    int8_t RELe();
    uint16_t ABSe();
    uint16_t ABSXe();
    uint16_t ABSYe();
    uint16_t INDe();
    uint16_t INDXe();
    uint16_t INDYe();

    // Opcodes
    void ADCe(); void ANDe();

    // Because addressing mode affects how many cycles each opcode takes, I made these to map them together
    // Addressing Modes
    enum AddressingModes {

        IMP, // Implicit or Implied
        ACC, // Accumulator
        IMM, // Immediate
        ZERO, // Zero Page
        ZEROX, // Zero Page, X
        ZEROY, // Zero Page, Y
        REL, // Relative
        ABS, // Absolute
        ABSX, // Absolute, X
        ABSY, // Absolute, Y
        IND, // Indirect
        INDX, // INDXd Indirect (INDIRECT, X)
        INDY // Indirect INDXd (INDIRECT, Y)
    };

    // Opcodes
    enum instructions {
        ADC, AND, ASL, BCC, BCS, BEQ, BIT,
        BMI, BNE, BPL, BRK, BVC, BVS, CLC,
        CLD, CLI, CLV, CMP, CPX, CPY, DEC,
        DEX, DEY, EOR, INC, INX, INY, JMP,
        JSR, LDA, LDX, LDY, LSR, NOP, ORA,
        PHA, PHP, PLA, PLP, ROL, ROR, RTI,
        RTS, SBC, SEC, SED, SEI, STA, STX,
        STY, TAX, TAY, TSX, TXA, TXS, TYA
    };

    // For Lookup Table
    struct OpcodeInfo {
        instructions instruction;
        AddressingModes addressingMode;
        int cycles;
        byte origOpcode;
        //std::string origOpcode;
        std::string convertedOpcode;
    };

    // Because addressing modes just affect the way an op is read and the number of cycles, I connected them
    // This *does not* account for page crosses, I did that in .cpp file
    // Each row represents an opcode
    // https://www.nesdev.org/obelisk-6502-guide/reference.html

    const std::vector<OpcodeInfo> opcodeTable = {
    OpcodeInfo{ADC, IMM, 2, 0x69, "ADC"}, OpcodeInfo{ADC, ZERO, 3, 0x65, "ADC"}, OpcodeInfo{ADC, ZEROX, 4, 0x75, "ADC"}, OpcodeInfo{ADC, ABS, 4, 0x6d, "ADC"}, OpcodeInfo{ADC, ABSX, 4, 0x7d, "ADC"}, OpcodeInfo{ADC, ABSY, 4, 0x79, "ADC"}, OpcodeInfo{ADC, INDX, 6, 0x61, "ADC"}, OpcodeInfo{ADC, INDY, 5, 0x71, "ADC"},
    OpcodeInfo{AND, IMM, 2, 0x29, "AND"}, OpcodeInfo{AND, ZERO, 3, 0x25, "AND"}, OpcodeInfo{AND, ZEROX, 4, 0x35, "AND"}, OpcodeInfo{AND, ABS, 4, 0x2d, "AND"}, OpcodeInfo{AND, ABSX, 4, 0x3d, "AND"}, OpcodeInfo{AND, ABSY, 4, 0x39, "AND"}, OpcodeInfo{AND, INDX, 6, 0x21, "AND"}, OpcodeInfo{AND, INDY, 5, 0x31, "AND"},
    OpcodeInfo{ASL, ACC, 2, 0x0a, "ASL"}, OpcodeInfo{ASL, ZERO, 5, 0x06, "ASL"}, OpcodeInfo{ASL, ZEROX, 6, 0x16, "ASL"}, OpcodeInfo{ASL, ABS, 6, 0x0e, "ASL"}, OpcodeInfo{ASL, ABSX, 7, 0x1e, "ASL"},
    OpcodeInfo{BCC, REL, 2, 0x90, "BCC"},
    OpcodeInfo{BCS, REL, 2, 0xb0, "BCS"},
    OpcodeInfo{BEQ, REL, 2, 0xf0, "BEQ"},
    OpcodeInfo{BIT, ZERO, 3, 0x24, "BIT"}, OpcodeInfo{BIT, ABS, 4, 0x2c, "BIT"},
    OpcodeInfo{BMI, REL, 2, 0x30, "BMI"},
    OpcodeInfo{BNE, REL, 2, 0xd0, "BNE"},
    OpcodeInfo{BPL, REL, 2, 0x10, "BPL"},
    OpcodeInfo{BRK, IMP, 7, 0x00, "BRK"},
    OpcodeInfo{BVC, REL, 2, 0x50, "BVC"},
    OpcodeInfo{BVS, REL, 2, 0x70, "BVS"},
    OpcodeInfo{CLC, IMP, 2, 0x18, "CLC"},
    OpcodeInfo{CLD, IMP, 2, 0xd8, "CLD"},
    OpcodeInfo{CLI, IMP, 2, 0x58, "CLI"},
    OpcodeInfo{CLV, IMP, 2, 0xb8, "CLV"},
    OpcodeInfo{CMP, IMM, 2, 0xc9, "CMP"}, OpcodeInfo{CMP, ZERO, 3, 0xc5, "CMP"}, OpcodeInfo{CMP, ZEROX, 4, 0xd5, "CMP"}, OpcodeInfo{CMP, ABS, 4, 0xcd, "CMP"}, OpcodeInfo{CMP, ABSX, 4, 0xdd, "CMP"}, OpcodeInfo{CMP, ABSY, 4, 0xd9, "CMP"}, OpcodeInfo{CMP, INDX, 6, 0xc1, "CMP"}, OpcodeInfo{CMP, INDY, 5, 0xd1, "CMP"},
    OpcodeInfo{CPX, IMM, 2, 0xe0, "CPX"}, OpcodeInfo{CPX, ZERO, 3, 0xe4, "CPX"}, OpcodeInfo{CPX, ABS, 4, 0xec, "CPX"},
    OpcodeInfo{CPY, IMM, 2, 0xc0, "CPY"}, OpcodeInfo{CPY, ZERO, 3, 0xc4, "CPY"}, OpcodeInfo{CPY, ABS, 4, 0xcc, "CPY"},
    OpcodeInfo{DEC, ZERO, 5, 0xc6, "DEC"}, OpcodeInfo{DEC, ZEROX, 6, 0xd6, "DEC"}, OpcodeInfo{DEC, ABS, 6, 0xce, "DEC"}, OpcodeInfo{DEC, ABSX, 7, 0xde, "DEC"},
    OpcodeInfo{DEX, IMP, 2, 0xca, "DEX"},
    OpcodeInfo{DEY, IMP, 2, 0x88, "DEY"},
    OpcodeInfo{EOR, IMM, 2, 0x49, "EOR"}, OpcodeInfo{EOR, ZERO, 3, 0x45, "EOR"}, OpcodeInfo{EOR, ZEROX, 4, 0x55, "EOR"}, OpcodeInfo{EOR, ABS, 4, 0x4d, "EOR"}, OpcodeInfo{EOR, ABSX, 4, 0x5d, "EOR"}, OpcodeInfo{EOR, ABSY, 4, 0x59, "EOR"}, OpcodeInfo{EOR, INDX, 6, 0x41, "EOR"}, OpcodeInfo{EOR, INDY, 5, 0x51, "EOR"},
    OpcodeInfo{INC, ZERO, 5, 0xe6, "INC"}, OpcodeInfo{INC, ZEROX, 6, 0xf6, "INC"}, OpcodeInfo{INC, ABS, 6, 0xee, "INC"}, OpcodeInfo{INC, ABSX, 7, 0xfe, "INC"},
    OpcodeInfo{INX, IMP, 2, 0xe8, "INX"},
    OpcodeInfo{INY, IMP, 2, 0xc8, "INY"},
    OpcodeInfo{JMP, ABS, 3, 0x4c, "JMP"}, OpcodeInfo{JMP, IND, 5, 0x6c, "JMP"},
    OpcodeInfo{JSR, ABS, 6, 0x20, "JSR"},
    OpcodeInfo{LDA, IMM, 2, 0xa9, "LDA"}, OpcodeInfo{LDA, ZERO, 3, 0xa5, "LDA"}, OpcodeInfo{LDA, ZEROX, 4, 0xb5, "LDA"}, OpcodeInfo{LDA, ABS, 4, 0xad, "LDA"}, OpcodeInfo{LDA, ABSX, 4, 0xbd, "LDA"}, OpcodeInfo{LDA, ABSY, 4, 0xb9, "LDA"}, OpcodeInfo{LDA, INDX, 6, 0xa1, "LDA"}, OpcodeInfo{LDA, INDY, 6, 0xb1, "LDA"},
    OpcodeInfo{LDX, IMM, 2, 0xa2, "LDX"}, OpcodeInfo{LDX, ZERO, 3, 0xa6, "LDX"}, OpcodeInfo{LDX, ZEROY, 4, 0xb6, "LDX"}, OpcodeInfo{LDX, ABS, 4, 0xae, "LDX"}, OpcodeInfo{LDX, ABSY, 4, 0xbe, "LDX"},
    OpcodeInfo{LDY, IMM, 2, 0xa0, "LDY"}, OpcodeInfo{LDY, ZERO, 3, 0xa4, "LDY"}, OpcodeInfo{LDY, ZEROX, 4, 0xb4, "LDY"}, OpcodeInfo{LDY, ABS, 4, 0xac, "LDY"}, OpcodeInfo{LDY, ABSX, 4, 0xbc, "LDY"},
    OpcodeInfo{LSR, ACC, 2, 0x4a, "LSR"}, OpcodeInfo{LSR, ZERO, 5, 0x46, "LSR"}, OpcodeInfo{LSR, ZEROX, 6, 0x56, "LSR"}, OpcodeInfo{LSR, ABS, 6, 0x4e, "LSR"}, OpcodeInfo{LSR, ABSX, 7, 0x5e, "LSR"},
    OpcodeInfo{NOP, IMP, 2, 0xea, "NOP"},
    OpcodeInfo{ORA, IMM, 2, 0x09, "ORA"}, OpcodeInfo{ORA, ZERO, 3, 0x05, "ORA"}, OpcodeInfo{ORA, ZEROX, 4, 0x15, "ORA"}, OpcodeInfo{ORA, ABS, 4, 0x0d, "ORA"}, OpcodeInfo{ORA, ABSX, 4, 0x1d, "ORA"}, OpcodeInfo{ORA, ABSY, 4, 0x19, "ORA"}, OpcodeInfo{ORA, INDX, 6, 0x01, "ORA"}, OpcodeInfo{ORA, INDY, 5, 0x11, "ORA"},
    OpcodeInfo{PHA, IMP, 3, 0x48, "PHA"},
    OpcodeInfo{PHP, IMP, 3, 0x08, "PHP"},
    OpcodeInfo{PLA, IMP, 4, 0x68, "PLA"},
    OpcodeInfo{PLP, IMP, 4, 0x28, "PLP"},
    OpcodeInfo{ROL, ACC, 2, 0x2a, "ROL"}, OpcodeInfo{ROL, ZERO, 5, 0x26, "ROL"}, OpcodeInfo{ROL, ZEROX, 6, 0x36, "ROL"}, OpcodeInfo{ROL, ABS, 6, 0x2e, "ROL"}, OpcodeInfo{ROL, ABSX, 7, 0x3e, "ROL"},
    OpcodeInfo{ROR, ACC, 2, 0x6a, "ROR"}, OpcodeInfo{ROR, ZERO, 5, 0x66, "ROR"}, OpcodeInfo{ROR, ZEROX, 6, 0x76, "ROR"}, OpcodeInfo{ROR, ABS, 6, 0x6e, "ROR"}, OpcodeInfo{ROR, ABSX, 7, 0x7e, "ROR"},
    OpcodeInfo{RTI, IMP, 6, 0x40, "RTI"},
    OpcodeInfo{RTS, IMP, 6, 0x60, "RTS"},
    OpcodeInfo{SBC, IMM, 2, 0xe9, "SBC"}, OpcodeInfo{SBC, ZERO, 3, 0xe5, "SBC"}, OpcodeInfo{SBC, ZEROX, 4, 0xf5, "SBC"}, OpcodeInfo{SBC, ABS, 4, 0xed, "SBC"}, OpcodeInfo{SBC, ABSX, 4, 0xfd, "SBC"}, OpcodeInfo{SBC, ABSY, 4, 0xf9, "SBC"}, OpcodeInfo{SBC, INDX, 6, 0xe1, "SBC"}, OpcodeInfo{SBC, INDY, 5, 0xf1, "SBC"},
    OpcodeInfo{SEC, IMP, 2, 0x38, "SEC"},
    OpcodeInfo{SED, IMP, 2, 0xf8, "SED"},
    OpcodeInfo{SEI, IMP, 2, 0x78, "SEI"},
    OpcodeInfo{STA, ZERO, 3, 0x85, "STA"}, OpcodeInfo{STA, ZEROX, 4, 0x95, "STA"}, OpcodeInfo{STA, ABS, 4, 0x8d, "STA"}, OpcodeInfo{STA, ABSX, 5, 0x9d, "STA"}, OpcodeInfo{STA, ABSY, 5, 0x99, "STA"}, OpcodeInfo{STA, INDX, 6, 0x81, "STA"}, OpcodeInfo{STA, INDY, 6, 0x91, "STA"},
    OpcodeInfo{STX, ZERO, 3, 0x86, "STX"}, OpcodeInfo{STX, ZEROY, 4, 0x96, "STX"}, OpcodeInfo{STX, ABS, 4, 0x8e, "STX"},
    OpcodeInfo{STY, ZERO, 3, 0x84, "STY"}, OpcodeInfo{STY, ZEROX, 4, 0x94, "STY"}, OpcodeInfo{STY, ABS, 4, 0x8c, "STY"},
    OpcodeInfo{TAX, IMP, 2, 0xaa, "TAX"},
    OpcodeInfo{TAY, IMP, 2, 0xa8, "TAY"},
    OpcodeInfo{TSX, IMP, 2, 0xba, "TSX"},
    OpcodeInfo{TXA, IMP, 2, 0x8a, "TXA"},
    OpcodeInfo{TXS, IMP, 2, 0x9a, "TXS"},
    OpcodeInfo{TYA, IMP, 2, 0x98, "TYA"},

    };
    std::unordered_map<AddressingModes, AddressingModeHandler> addressingModeHandlers;

    void initAddressingModeHandlers() {
        addressingModeHandlers = {
            // 8-bit casted onto 16-bit ex: 0x12 turns into 0x0012
            // {AddressingModes::IMP, [this]() { return static_cast<uint16_t>(IMPe()); }},
            // {AddressingModes::ACC, [this]() { return static_cast<uint16_t>(ACCe()); }},
            {AddressingModes::IMM, [this]() { return static_cast<uint16_t>(IMMe()); }},
            {AddressingModes::ZERO, [this]() { return static_cast<uint16_t>(ZEROe()); }},
            {AddressingModes::ZEROX, [this]() { return static_cast<uint16_t>(ZEROXe()); }},
            {AddressingModes::ZEROY, [this]() { return static_cast<uint16_t>(ZEROYe()); }},
            // {AddressingModes::REL, [this]() { return static_cast<int8_t>(RELe()); }}, // Note: Example assumes false for condition
            {AddressingModes::ABS, [this]() { return ABSe(); }},
            {AddressingModes::ABSX, [this]() { return ABSXe(); }},
            {AddressingModes::ABSY, [this]() { return ABSYe(); }},
            {AddressingModes::IND, [this]() { return INDe(); }},
            {AddressingModes::INDX, [this]() { return INDXe(); }},
            {AddressingModes::INDY, [this]() { return INDYe(); }},

        };
    }
    int8_t relAddr;

    // Main Loop
    void run();
    // Interrupts
    void nmi();
    void irq();
    // Inerrupt
    void interrupt();
    // Fetch instruction (or part of instruction)
    byte fetch();
    void decodeAndExecute(byte instruction, int& cycles, AddressingModes addressingMode);

    //Read from binary file, load to memory
    void loadToMemory(const std::vector<byte>& data, uint16_t startAddress);

    //Read from test file
    void loadIntelHexFile(const std::string& filename);

    int cycles = 0;
    instructions instruction;
    AddressingModes addressingMode;

    void pushByteToStack(byte value);

    long getCurrentTime();

    byte pop();

    // Instruction helper variables
    bool overflow;
    byte ADCresult;
    unsigned int sum;
    byte BITresult;
    byte CMPresult;
    byte CPXresult;
    byte CPYresult;
    uint16_t returnAddress;
    byte newStatus;
    byte RTIhigh;
    byte RTIlow;
    byte RTShigh;
    byte RTSlow;
    byte newStatusRTI;
    byte effectiveValue;
    byte difference;
    uint16_t tempAddr;
    uint16_t operandAddress;
    byte operandValue = 0;
    int printCount = 0;
    int pc2 = 0;

    int cycleCount;

    const int CPU_FREQUENCY = 1790000; // For NES, 1.79 MHz
    bool pageCrossed;

    uint16_t temppc;

    bool debug = false;

    byte fetchValue();
    byte opValue;

    uint16_t tempidx;

    void setPCStartup() {
        pc = bus->CpuPcStart();
    }
    
};




#endif // CPU_H