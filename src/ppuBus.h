#ifndef PPUBUS_H
#define PPUBUS_H
#include <cstdint>
#include <vector>
#include "singleton.h"

typedef uint8_t byte;
class ppuBus : public Singleton<ppuBus> {
	friend class Singleton<ppuBus>;
public:

};

#endif