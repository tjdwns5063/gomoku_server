#pragma once

#include <vector>
#include <stdexcept>

class SendBuffer
{
private:
	std::vector<char> buffer;
	size_t writeSize;

public:
	SendBuffer(size_t capacity);

	void write(const char* data, size_t len);
	char* getBufferPtr();
	int getWriteSize() const;
};
