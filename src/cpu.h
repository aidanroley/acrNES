
#include "bus.h"

typedef uint8_t byte;

class cpu {
private:
    Bus bus;



public:
    cpu();
    ~cpu();

    // 8-bit accumulators, status register, stack pointer
    byte a = 0, x = 0, y = 0, status = 0, stackpt = 0xFF;

    // 16-bit program counter; it points to the address at which next instruction will be fetched
    // The program counter may be read by pushing its value onto the stack  
    uint16_t pc = 0x0400;

    uint16_t absolute;
    uint16_t temp_addr;
    byte operand;
    byte zeroPageAddr;
    byte relative;
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
    byte RELe();
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
        std::string origOpcode;
        std::string convertedOpcode;
    };

    // Because addressing modes just affect the way an op is read and the number of cycles, I connected them
    // This *does not* account for page crosses, I did that in .cpp file
    // Each row represents an opcode
    // https://www.nesdev.org/obelisk-6502-guide/reference.html

    const std::vector<OpcodeInfo> opcodeTable = {
    OpcodeInfo{ADC, IMM, 2, "69", "ADC"}, OpcodeInfo{ADC, ZERO, 3, "65", "ADC"}, OpcodeInfo{ADC, ZEROX, 4, "75", "ADC"}, OpcodeInfo{ADC, ABS, 4, "6D", "ADC"}, OpcodeInfo{ADC, ABSX, 4, "7D", "ADC"}, OpcodeInfo{ADC, ABSY, 4, "79", "ADC"}, OpcodeInfo{ADC, INDX, 6, "61", "ADC"}, OpcodeInfo{ADC, INDY, 5, "71", "ADC"},
    OpcodeInfo{AND, IMM, 2, "29", "AND"}, OpcodeInfo{AND, ZERO, 3, "25", "AND"}, OpcodeInfo{AND, ZEROX, 4, "35", "AND"}, OpcodeInfo{AND, ABS, 4, "2D", "AND"}, OpcodeInfo{AND, ABSX, 4, "3D", "AND"}, OpcodeInfo{AND, ABSY, 4, "39", "AND"}, OpcodeInfo{AND, INDX, 6, "21", "AND"}, OpcodeInfo{AND, INDY, 5, "31", "AND"},
    OpcodeInfo{ASL, ACC, 2, "0A", "ASL"}, OpcodeInfo{ASL, ZERO, 5, "06", "ASL"}, OpcodeInfo{ASL, ZEROX, 6, "16", "ASL"}, OpcodeInfo{ASL, ABS, 6, "0E", "ASL"}, OpcodeInfo{ASL, ABSX, 7, "1E", "ASL"},
    OpcodeInfo{BCC, REL, 2, "90", "BCC"},
    OpcodeInfo{BCS, REL, 2, "B0", "BCS"},
    OpcodeInfo{BEQ, REL, 2, "F0", "BEQ"},
    OpcodeInfo{BIT, ZERO, 3, "24", "BIT"}, OpcodeInfo{BIT, ABS, 4, "2C", "BIT"},
    OpcodeInfo{BMI, REL, 2, "30", "BMI"},
    OpcodeInfo{BNE, REL, 2, "D0", "BNE"},
    OpcodeInfo{BPL, REL, 2, "10", "BPL"},
    OpcodeInfo{BRK, IMP, 7, "00", "BRK"},
    OpcodeInfo{BVC, REL, 2, "50", "BVC"},
    OpcodeInfo{BVS, REL, 2, "70", "BVS"},
    OpcodeInfo{CLC, IMP, 2, "18", "CLC"},
    OpcodeInfo{CLD, IMP, 2, "D8", "CLD"},
    OpcodeInfo{CLI, IMP, 2, "58", "CLI"},
    OpcodeInfo{CLV, IMP, 2, "B8", "CLV"},
    OpcodeInfo{CMP, IMM, 2, "C9", "CMP"}, OpcodeInfo{CMP, ZERO, 3, "C5", "CMP"}, OpcodeInfo{CMP, ZEROX, 4, "D5", "CMP"}, OpcodeInfo{CMP, ABS, 4, "CD", "CMP"}, OpcodeInfo{CMP, ABSX, 4, "DD", "CMP"}, OpcodeInfo{CMP, ABSY, 4, "D9", "CMP"}, OpcodeInfo{CMP, INDX, 6, "C1", "CMP"}, OpcodeInfo{CMP, INDY, 5, "D1", "CMP"},
    OpcodeInfo{CPX, IMM, 2, "E0", "CPX"}, OpcodeInfo{CPX, ZERO, 3, "E4", "CPX"}, OpcodeInfo{CMP, ABS, 4, "EC", "CPX"},
    OpcodeInfo{CPY, IMM, 2, "C0", "CPY"}, OpcodeInfo{CPY, ZERO, 3, "C4", "CPY"}, OpcodeInfo{CPY, ABS, 4, "CC", "CPY"},
    OpcodeInfo{DEC, ZERO, 5, "C6", "DEC"}, OpcodeInfo{DEC, ZEROX, 6, "D6", "DEC"}, OpcodeInfo{DEC, ABS, 6, "CE", "DEC"}, OpcodeInfo{DEC, ABSX, 7, "DE", "DEC"},
    OpcodeInfo{DEX, IMP, 2, "CA", "DEX"},
    OpcodeInfo{DEY, IMP, 2, "88", "DEY"},
    OpcodeInfo{EOR, IMM, 2, "49", "EOR"}, OpcodeInfo{EOR, ZERO, 3, "45", "EOR"}, OpcodeInfo{EOR, ZEROX, 4, "55", "EOR"}, OpcodeInfo{EOR, ABS, 4, "4D", "EOR"}, OpcodeInfo{EOR, ABSX, 4, "5D", "EOR"}, OpcodeInfo{EOR, ABSY, 4, "59", "EOR"}, OpcodeInfo{EOR, INDX, 6, "41", "EOR"}, OpcodeInfo{EOR, INDY, 5, "51", "EOR"},
    OpcodeInfo{INC, ZERO, 5, "E6", "INC"}, OpcodeInfo{INC, ZEROX, 6, "F6", "INC"}, OpcodeInfo{INC, ABS, 6, "EE", "INC"}, OpcodeInfo{INC, ABSX, 7, "FE", "INC"},
    OpcodeInfo{INX, IMP, 2, "E8", "INX"},
    OpcodeInfo{INY, IMP, 2, "C8", "INY"},
    OpcodeInfo{JMP, ABS, 3, "4C", "JMP"}, OpcodeInfo{JMP, IND, 5, "6C", "JMP"},
    OpcodeInfo{JSR, ABS, 6, "20", "JSR"},
    OpcodeInfo{LDA, IMM, 2, "A9", "LDA"}, OpcodeInfo{LDA, ZERO, 3, "A5", "LDA"}, OpcodeInfo{LDA, ZEROX, 4, "B5", "LDA"}, OpcodeInfo{LDA, ABS, 4, "AD", "LDA"}, OpcodeInfo{LDA, ABSX, 4, "BD", "LDA"}, OpcodeInfo{LDA, ABSY, 4, "B9", "LDA"}, OpcodeInfo{LDA, INDX, 6, "A1", "LDA"}, OpcodeInfo{LDA, INDY, 6, "B1", "LDA"},
    OpcodeInfo{LDX, IMM, 2, "A2", "LDX"}, OpcodeInfo{LDX, ZERO, 3, "A6", "LDX"}, OpcodeInfo{LDX, ZEROY, 4, "B6", "LDX"}, OpcodeInfo{LDX, ABS, 4, "AE", "LDX"}, OpcodeInfo{LDX, ABSY, 4, "BE", "LDX"},
    OpcodeInfo{LDY, IMM, 2, "A0", "LDY"}, OpcodeInfo{LDY, ZERO, 3, "A4", "LDY"}, OpcodeInfo{LDY, ZEROX, 4, "B4", "LDY"}, OpcodeInfo{LDY, ABS, 4, "AC", "LDY"}, OpcodeInfo{LDY, ABSX, 4, "BC", "LDY"},
    OpcodeInfo{LSR, ACC, 2, "4A", "LSR"}, OpcodeInfo{LSR, ZERO, 5, "46", "LSR"}, OpcodeInfo{LSR, ZEROX, 6, "56", "LSR"}, OpcodeInfo{LSR, ABS, 6, "4E", "LSR"}, OpcodeInfo{LSR, ABSX, 7, "5E", "LSR"},
    OpcodeInfo{NOP, IMP, 2, "EA", "NOP"},
    OpcodeInfo{ORA, IMM, 2, "09", "ORA"}, OpcodeInfo{ORA, ZERO, 3, "05", "ORA"}, OpcodeInfo{ORA, ZEROX, 4, "15", "ORA"}, OpcodeInfo{ORA, ABS, 4, "0D", "ORA"}, OpcodeInfo{ORA, ABSX, 4, "1D", "ORA"}, OpcodeInfo{ORA, ABSY, 4, "19", "ORA"}, OpcodeInfo{ORA, INDX, 6, "01", "ORA"}, OpcodeInfo{ORA, INDY, 5, "11", "ORA"},
    OpcodeInfo{PHA, IMP, 3, "48", "PHA"},
    OpcodeInfo{PHP, IMP, 3, "08", "PHP"},
    OpcodeInfo{PLA, IMP, 4, "68", "PLA"},
    OpcodeInfo{PLP, IMP, 4, "28", "PLP"},
    OpcodeInfo{ROL, ACC, 2, "2A", "ROL"}, OpcodeInfo{ROL, ZERO, 5, "26", "ROL"}, OpcodeInfo{ROL, ZEROX, 6, "36", "ROL"}, OpcodeInfo{ROL, ABS, 6, "2E", "ROL"}, OpcodeInfo{ROL, ABSX, 7, "3E", "ROL"},
    OpcodeInfo{ROR, ACC, 2, "6A", "ROR"}, OpcodeInfo{ROR, ZERO, 5, "66", "ROR"}, OpcodeInfo{ROR, ZEROX, 6, "76", "ROR"}, OpcodeInfo{ROR, ABS, 6, "6E", "ROR"}, OpcodeInfo{ROR, ABSX, 7, "7E", "ROR"},
    OpcodeInfo{RTI, IMP, 6, "40", "RTI"},
    OpcodeInfo{RTS, IMP, 6, "60", "RTS"},
    OpcodeInfo{SBC, IMM, 2, "E9", "SBC"}, OpcodeInfo{SBC, ZERO, 3, "E5", "SBC"}, OpcodeInfo{SBC, ZEROX, 4, "F5", "SBC"}, OpcodeInfo{SBC, ABS, 4, "ED", "SBC"}, OpcodeInfo{SBC, ABSX, 4, "FD", "SBC"}, OpcodeInfo{SBC, ABSY, 4, "F9", "SBC"}, OpcodeInfo{SBC, INDX, 6, "E1", "SBC"}, OpcodeInfo{SBC, INDY, 5, "F1", "SBC"},
    OpcodeInfo{SEC, IMP, 2, "38", "SEC"},
    OpcodeInfo{SED, IMP, 2, "F8", "SED"},
    OpcodeInfo{SEI, IMP, 2, "78", "SEI"},
    OpcodeInfo{STA, ZERO, 3, "85", "STA"}, OpcodeInfo{STA, ZEROX, 4, "95", "STA"}, OpcodeInfo{STA, ABS, 4, "8D", "STA"}, OpcodeInfo{STA, ABSX, 5, "9D", "STA"}, OpcodeInfo{STA, ABSY, 5, "99", "STA"}, OpcodeInfo{STA, INDX, 6, "81", "STA"}, OpcodeInfo{STA, INDY, 6, "91", "STA"},
    OpcodeInfo{STX, ZERO, 3, "86", "STX"}, OpcodeInfo{STX, ZEROY, 4, "96", "STX"}, OpcodeInfo{STX, ABS, 4, "8E", "STX"},
    OpcodeInfo{STY, ZERO, 3, "84", "STY"}, OpcodeInfo{STY, ZEROX, 4, "94", "STY"}, OpcodeInfo{STY, ABS, 4, "8C", "STY"},
    OpcodeInfo{TAX, IMP, 2, "AA", "TAX"},
    OpcodeInfo{TAY, IMP, 2, "A8", "TAY"},
    OpcodeInfo{TSX, IMP, 2, "BA", "TSX"},
    OpcodeInfo{TXA, IMP, 2, "8A", "TXA"},
    OpcodeInfo{TXS, IMP, 2, "9A", "TXS"},
    OpcodeInfo{TYA, IMP, 2, "98", "TYA"},

    };
    std::unordered_map<AddressingModes, AddressingModeHandler> addressingModeHandlers;

    void initAddressingModeHandlers() {
        addressingModeHandlers = {
            // 8-bit casted onto 16-bit ex: 0x12 turns into 0x0012
            {AddressingModes::IMP, [this]() { return static_cast<uint16_t>(IMPe()); }},
            {AddressingModes::ACC, [this]() { return static_cast<uint16_t>(ACCe()); }},
            {AddressingModes::IMM, [this]() { return static_cast<uint16_t>(IMMe()); }},
            {AddressingModes::ZERO, [this]() { return static_cast<uint16_t>(ZEROe()); }},
            {AddressingModes::ZEROX, [this]() { return static_cast<uint16_t>(ZEROXe()); }},
            {AddressingModes::ZEROY, [this]() { return static_cast<uint16_t>(ZEROYe()); }},
            {AddressingModes::REL, [this]() { return static_cast<uint16_t>(RELe()); }}, // Note: Example assumes false for condition
            {AddressingModes::ABS, [this]() { return ABSe(); }},
            {AddressingModes::ABSX, [this]() { return ABSXe(); }},
            {AddressingModes::ABSY, [this]() { return ABSYe(); }},
            {AddressingModes::IND, [this]() { return INDe(); }},
            {AddressingModes::INDX, [this]() { return INDXe(); }},
            {AddressingModes::INDY, [this]() { return INDYe(); }},

        };
    }


    // Main Loop
    void run();
    // Inerrupt
    void interrupt();
    // Fetch instruction (or part of instruction)
    byte fetch();
    void decodeAndExecute(byte instruction, int cycles, AddressingModes addressingMode);

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
    unsigned int effectiveValue;
    unsigned int difference;

    int printCount = 0;
    int pc2 = 0;

    int cycleCount;

    const int CPU_FREQUENCY = 1790000; // For NES, 1.79 MHz
    bool pageCrossed;

    uint16_t temppc;
    
};

cpu::cpu() {
    // Constructor body
}

cpu::~cpu() {
    // Destructor body
}