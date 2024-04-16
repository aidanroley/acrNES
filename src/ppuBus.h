#ifndef PPUBUS_H
#define PPUBUS_H
#include <cstdint>
#include <vector>
#include "singleton.h"

typedef uint8_t byte;
class ppuBus : public Singleton<ppuBus> {
	friend class Singleton<ppuBus>;
public:
	std::vector<byte> ppuCHR;
	void initializeCHR(const std::vector<uint8_t>& chr);

	void checkPpuBus();
};

#endif