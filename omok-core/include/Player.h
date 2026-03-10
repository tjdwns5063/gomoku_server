#pragma once

#include <cstdint>

class Player {
private:
	uint32_t id;

public:
	Player(uint32_t id);
	uint32_t getId() const;
};
