#pragma once

#include <cstdint>
#include <atomic>

class IdGenerator
{
private:
	std::atomic<uint64_t> currentId;

public:
	IdGenerator();
	~IdGenerator();

	uint64_t generateId();
	uint64_t getCurrentId() const;
};

