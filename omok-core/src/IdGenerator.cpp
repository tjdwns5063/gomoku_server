#include "IdGenerator.h"

IdGenerator::IdGenerator() : currentId(0) {}
IdGenerator::~IdGenerator() {}

uint32_t IdGenerator::generateId() {
	return currentId.fetch_add(1, std::memory_order_relaxed) + 1;
}

uint32_t IdGenerator::getCurrentId() const {
	return currentId.load(std::memory_order_relaxed);
}