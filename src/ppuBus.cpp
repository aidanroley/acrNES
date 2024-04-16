#include "ppuBus.h"
#include "bus.h"

void ppuBus::initializeCHR(const std::vector<uint8_t>& chr) {
	ppuCHR = chr;

    std::cout << "CHR Data: ";
    for (const auto& value : ppuCHR) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(value) << " ";
    }
    std::cout << std::endl;

    std::cout << std::dec << "Size of ppuCHR in bytes: " << ppuCHR.size() << std::endl;
}

void ppuBus::checkPpuBus() {
    std::cout << "CHR PLEASE WORK!!!" << std::endl;
    for (const auto& value : ppuCHR) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(value) << " ";
    }
}