#pragma once

#include <cstdint>
#include <atomic>

class IdGenerator
{
private:
	std::atomic<uint32_t> currentId;

public:
	IdGenerator();
	~IdGenerator();

	uint32_t generateId();
	uint32_t getCurrentId() const;
};

