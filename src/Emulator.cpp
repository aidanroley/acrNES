#include "PPU.h"
#include "ROM.h"
#include "cpu.h"
#include "Emulator.h"

ROM romMain;
void main() {
	Emulator emulator;
	emulator.start();
}
void Emulator::start() {
	int val = romMain.parseFile();
	ppuBus->checkPpuBus();
}