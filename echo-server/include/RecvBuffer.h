#pragma once

#include <vector>
#include <cstdint>

class RecvBuffer
{
private:
	std::vector<char> buffer;
	size_t writePos; // 버퍼에 데이터를 쓸 위치
	size_t readPos;
	size_t capacity;

public:
	RecvBuffer(size_t size);
	size_t getDataSize() const;
	size_t getFreeSize() const;
	char* readData();
	void clean();
	char* getWritePtr();
	char* getReadPtr();
	bool onRead(size_t len);
	bool onWrite(size_t len);
};

