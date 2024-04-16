#include "ppuBus.h"
#include "bus.h"

void ppuBus::checkPpuBus() {
    std::cout << "CHR PLEASE WORK!!!" << std::endl;
    for (const auto& value : ppuCHR) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(value) << " ";
    }
}