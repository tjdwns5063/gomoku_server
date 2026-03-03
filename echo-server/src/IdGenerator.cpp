#include "IdGenerator.h"

IdGenerator::IdGenerator() : currentId(0) {}
IdGenerator::~IdGenerator() {}

uint64_t IdGenerator::generateId() {
	return currentId.fetch_add(1, std::memory_order_relaxed);
}

uint64_t IdGenerator::getCurrentId() const {
	return currentId.load(std::memory_order_relaxed);
}